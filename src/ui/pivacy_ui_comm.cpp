/*
 * Copyright (c) 2013 Roland van Rijswijk-Deij
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************
 pivacy_ui_comm.h

 The Pivacy UI communications thread
 *****************************************************************************/

#include "config.h"
#include "pivacy_ui_comm.h"
#include "pivacy_ui_proto.h"
#include "pivacy_log.h"
#include "pivacy_ui_canvas.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#define PIVACY_UI_BACKLOG		5			/* number of pending connections in the backlog */

pivacy_ui_comm_thread::pivacy_ui_comm_thread(wxWindow* main_wnd) : wxThread(wxTHREAD_JOINABLE)
{
	should_run = false;
	this->main_wnd = main_wnd;
}

void pivacy_ui_comm_thread::drop_client(int& socket_fd)
{
	close(socket_fd);
	
	INFO_MSG("Client on socket %d disconnected", socket_fd);
	
	socket_fd = -1;
	
	pivacy_ui_event evt(PEVT_NOCLIENT);
	send_event_and_wait(evt);
}

std::string pivacy_ui_comm_thread::string_from_vector(std::vector<unsigned char>& vec)
{
	if (vec.size() == 0)
	{
		return "";
	}
	
	size_t len = vec[0];
	
	if (vec.size() < (len + 1))
	{
		vec.clear();
		return "";
	}
	
	std::string str = std::string((const char*) &vec[1], len);
	
	memcpy(&vec[0], &vec[len + 1], vec.size() - len - 1);
	vec.resize(vec.size() - (len + 1));

	return str;
}

void* pivacy_ui_comm_thread::Entry()
{
	DEBUG_MSG("Entering communications thread");

	/* Clear display */
	pivacy_ui_event clean_evt(PEVT_NOCLIENT);
	send_event_and_wait(clean_evt);
	
	/* Clean up lingering old socket */
	unlink(PIVACY_UI_SOCKET);
	
	/* Set up UNIX domain socket for communications */
	int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	
	if (socket_fd < 0)
	{
		ERROR_MSG("Fatal: unable to create a socket");
		
		return NULL;
	}
	
	DEBUG_MSG("Opened socket %d", socket_fd);
	
	struct sockaddr_un addr = { 0 };
	
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, UNIX_PATH_MAX, PIVACY_UI_SOCKET);
	
	if (bind(socket_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) != 0)
	{
		ERROR_MSG("Fatal: failed to bind socket to %s", PIVACY_UI_SOCKET);
		
		close(socket_fd);
		
		unlink(PIVACY_UI_SOCKET);
		
		return NULL;
	}
	
	INFO_MSG("Bound socket to %s", PIVACY_UI_SOCKET);
	
	if (listen(socket_fd, PIVACY_UI_BACKLOG) != 0)
	{
		ERROR_MSG("Fatal: failed to listen on socket %d (%s)", socket_fd, PIVACY_UI_SOCKET);
		
		close(socket_fd);
		
		unlink(PIVACY_UI_SOCKET);
		
		return NULL;
	}
	
	INFO_MSG("Socket listening for connection requests");
	
	int client_socket_fd = -1;
	
	while (should_run)
	{
		struct sockaddr_un peer;
		socklen_t peer_len = sizeof(struct sockaddr_un);
		fd_set wait_socks;
		
		/* Wait for incoming connections and commands on open connections */		
		while (should_run)
		{
			struct timeval timeout = { 0, 1000 }; // 1ms
			FD_ZERO(&wait_socks);
			FD_SET(socket_fd, &wait_socks);
			
			/* Add sockets for open connections */
			if (client_socket_fd != -1)
			{
				FD_SET(client_socket_fd, &wait_socks);
			}
	
			int rv = select(FD_SETSIZE, &wait_socks, NULL, NULL, &timeout);
			
			if (rv > 0) break;
		}
		
		if (!should_run) break;
		
		if (FD_ISSET(socket_fd, &wait_socks))
		{		
			/* Accept new connection */
			int new_client_fd = accept(socket_fd, (struct sockaddr*) &peer, &peer_len);
			
			if (new_client_fd >= 0)
			{
				if (client_socket_fd != -1)
				{
					ERROR_MSG("Connection attempt while another client is still active, closing socket");
					
					close(new_client_fd);
					
					continue;
				}
				
				INFO_MSG("New client socket %d open", new_client_fd);
				
				/* First, wait for the client to send the "request API version" command */
				std::vector<unsigned char> req_api_ver;
	
				if (!recv_from_client(new_client_fd, req_api_ver))
				{
					ERROR_MSG("Client on socket %d failed to send API version request", new_client_fd);
					
					close(new_client_fd);
					
					continue;
				}
				
				if ((req_api_ver.size() != 1) || (req_api_ver[0] != GET_API_VERSION))
				{
					ERROR_MSG("Client on socket %d uses invalid protocol, disconnecting client", new_client_fd);
					
					close(new_client_fd);
					
					continue;
				}
				
				std::vector<unsigned char> send_api_ver;
				send_api_ver.push_back((unsigned char) API_VERSION);
				
				if (!send_to_client(new_client_fd, send_api_ver))
				{
					ERROR_MSG("Failed to send API version to client on socket %d", new_client_fd);
					
					close(new_client_fd);
					
					continue;
				}
				
				INFO_MSG("Client connected successfully");
				
				client_socket_fd = new_client_fd;
			}
			else
			{
				switch(errno)
				{
				case EAGAIN:
					continue;
				case ECONNABORTED:
					WARNING_MSG("Incoming connection aborted");
					continue;
				case EINTR:
					WARNING_MSG("Interrupted by signal");
					break;
				default:
					ERROR_MSG("Error accepting new incoming connections (%d)", errno);
					break;
				}
			}
		}
		else
		{
			/* A message was received from the client */
			std::vector<unsigned char> client_cmd;
			
			if (!recv_from_client(client_socket_fd, client_cmd))
			{
				ERROR_MSG("Client communication error, closing client socket");
				
				drop_client(client_socket_fd);
				
				continue;
			}
			
			if (client_cmd.size() < 1)
			{
				ERROR_MSG("Invalid empty command received from client");
				
				continue;
			}
			
			switch(client_cmd[0])
			{
			case DISCONNECT:
				INFO_MSG("Client disconnected");
				
				drop_client(client_socket_fd);
				break;
			case SHOW_STATUS:
				{
					if (client_cmd.size() != 2)
					{
						ERROR_MSG("Invalid \"SHOW STATUS\" command from client");
						
						std::vector<unsigned char> resp;
						resp.push_back(PIVACY_UNKNOWN_CMD);
						
						if (!send_to_client(client_socket_fd, resp))
						{
							ERROR_MSG("Client communication error, closing client socket");
							
							drop_client(client_socket_fd);
							
							continue;
						}
					}
					
					DEBUG_MSG("Show status command for status %d", client_cmd[1]);
						
					pivacy_ui_event evt(PEVT_SHOWSTATUS);
					evt.set_show_status(client_cmd[1]);
					
					send_event_and_wait(evt);
					
					std::vector<unsigned char> resp;
					resp.push_back(PIVACY_OK);
					
					if (!send_to_client(client_socket_fd, resp))
					{
						ERROR_MSG("Client communication error, closing client socket");
						
						drop_client(client_socket_fd);
						
						continue;
					}
				}
				break;
			case SHOW_MESSAGE:
				{
					if (client_cmd.size() < 2)
					{
						ERROR_MSG("Invalid \"SHOW MESSAGE\" command from client");
						
						std::vector<unsigned char> resp;
						resp.push_back(PIVACY_UNKNOWN_CMD);
						
						if (!send_to_client(client_socket_fd, resp))
						{
							ERROR_MSG("Client communication error, closing client socket");
							
							drop_client(client_socket_fd);
							
							continue;
						}
					}
					
					std::string msg = std::string((const char*) &client_cmd[1], client_cmd.size() - 1);
					
					DEBUG_MSG("Request to display message \"%s\"", msg.c_str());
					
					pivacy_ui_event evt(PEVT_SHOWMSG);
					evt.set_show_message(msg);
					
					send_event_and_wait(evt);
					
					std::vector<unsigned char> resp;
					resp.push_back(PIVACY_OK);
					
					if (!send_to_client(client_socket_fd, resp))
					{
						ERROR_MSG("Client communication error, closing client socket");
						
						drop_client(client_socket_fd);
						
						continue;
					}
				}
				break;
			case REQUEST_PIN:
				{
					if (client_cmd.size() != 1)
					{
						ERROR_MSG("Invalid \"REQUEST PIN\" command from client");
						
						std::vector<unsigned char> resp;
						resp.push_back(PIVACY_UNKNOWN_CMD);
						
						if (!send_to_client(client_socket_fd, resp))
						{
							ERROR_MSG("Client communication error, closing client socket");
							
							drop_client(client_socket_fd);
							
							continue;
						}
					}
					
					DEBUG_MSG("Request to enter PIN");
					
					pivacy_ui_event_data evt_data;
					pivacy_ui_event evt(PEVT_REQUESTPIN, &evt_data);
					
					send_event_and_wait(evt);
					
					std::vector<unsigned char> resp;
					resp.resize(evt.get_pin().size() + 1);
					
					resp[0] = PIVACY_OK;
					memcpy(&resp[1], evt.get_pin().c_str(), evt.get_pin().size());
					
					if (!send_to_client(client_socket_fd, resp))
					{
						ERROR_MSG("Client communication error, closing client socket");
						
						drop_client(client_socket_fd);
						
						continue;
					}
				}
				break;
			case REQUEST_CONSENT:
				{
					DEBUG_MSG("Consent request");
					
					if (client_cmd.size() < 3)
					{
						ERROR_MSG("Invalid \"REQUEST CONSENT\" command from client");
						
						std::vector<unsigned char> resp;
						resp.push_back(PIVACY_UNKNOWN_CMD);
						
						if (!send_to_client(client_socket_fd, resp))
						{
							ERROR_MSG("Client communication error, closing client socket");
							
							drop_client(client_socket_fd);
							
							continue;
						}
					}
					
					bool show_always = (client_cmd[1] == 1);
					
					DEBUG_MSG("The always button in the consent dialog should %sbe shown", show_always ? "" : "not ");
					
					memcpy(&client_cmd[0], &client_cmd[2], client_cmd.size() - 2);
					client_cmd.resize(client_cmd.size() - 1);
					
					std::string rp_name = string_from_vector(client_cmd);
					
					DEBUG_MSG("Relying party asking consent: %s", rp_name.c_str());
					
					wxString wx_rp_name = wxString(rp_name.c_str(), wxConvUTF8);
					
					std::vector<wxString> rp_attr;
					
					while (client_cmd.size() > 0)
					{
						std::string attr_name = string_from_vector(client_cmd);
						
						rp_attr.push_back(wxString(attr_name.c_str(), wxConvUTF8));
						
						DEBUG_MSG("Adding attribute %s to consent request", attr_name.c_str());
					}
					
					pivacy_ui_event_data evt_data;
					pivacy_ui_event evt(PEVT_REQUESTCONSENT, &evt_data);
					
					evt.set_rp_name(wx_rp_name);
					evt.set_rp_attributes(rp_attr);
					evt.set_show_always(show_always);
					
					send_event_and_wait(evt);
					
					std::vector<unsigned char> resp;
					
					resp.push_back(PIVACY_OK);
					resp.push_back((unsigned char) evt.get_consent_result());
					
					if (!send_to_client(client_socket_fd, resp))
					{
						ERROR_MSG("Client communication error, closing client socket");
						
						drop_client(client_socket_fd);
						
						continue;
					}
				}
				break;
			default:
				{
					ERROR_MSG("Client sent unknown command %02X", client_cmd[0]);
					
					std::vector<unsigned char> resp;
					resp.push_back(PIVACY_UNKNOWN_CMD);
					
					if (!send_to_client(client_socket_fd, resp))
					{
						ERROR_MSG("Client communication error, closing client socket");
						
						drop_client(client_socket_fd);
						
						continue;
					}
				}
				break;
			}
		}
	}
	
	DEBUG_MSG("Closing socket");
	
	close(socket_fd);
	
	DEBUG_MSG("Exiting communications thread");
	
	return 0;
}

void pivacy_ui_comm_thread::send_event_and_wait(pivacy_ui_event& evt)
{
	wxMutex wait_mutex;
	wxCondition wait_cond(wait_mutex);
	
	wait_mutex.Lock();
	
	evt.set_mutex_and_cond(&wait_mutex, &wait_cond);
	
	main_wnd->AddPendingEvent(evt);
	
	wait_cond.Wait();
}

bool pivacy_ui_comm_thread::recv_from_client(int client_socket, std::vector<unsigned char>& rx)
{
	unsigned short rx_size = 0;
	
	size_t rx_index = 0;
	
	unsigned char buf[512] = { 0 };
	
	/*
	 * Read the length of the data to receive
	 * 
	 * FIXME: this will fail if the process receives a signal!
	 */
	if ((read(client_socket, &buf[0], 1) != 1) ||
	    (read(client_socket, &buf[1], 1) != 1))
	{
		return false;
	}
	
	rx_size = (buf[0] << 8) + buf[1];
	
	rx.resize(rx_size);
	
	/* 
	 * Now receive the actual data 
	 * 
	 * FIXME: again, will not deal well with POSIX signals!
	 */
	while (rx_size > 0)
	{
		int received = read(client_socket, &buf[0], (rx_size < 512) ? rx_size : 512);
		
		if (received < 0)
		{
			return false;
		}
		
		memcpy(&rx[rx_index], buf, received);
		
		rx_index += received;
		rx_size -= received;
	}
	
	return true;
}
	
bool pivacy_ui_comm_thread::send_to_client(int client_socket, std::vector<unsigned char>& tx)
{
	if (tx.size() > 0xffff) return -1;
	
	/* 
	 * Prepare the data for transmission by prepending a 16-bit
	 * value indicating the command length 
	 */
	unsigned short tx_size = (unsigned short) tx.size();
	
	std::vector<unsigned char> tx_buf;
	tx_buf.resize(tx.size() + 2);
	
	tx_buf[0] = tx_size >> 8;
	tx_buf[1] = tx_size & 0xff;
	
	memcpy(&tx_buf[2], &tx[0], tx.size());
	
	/* 
	 * Transmit the command
	 * 
	 * FIXME: this will fail if the process receives a POSIX signal! 
	 */
	int tx_sent = 0;
	 
	if ((tx_sent = write(client_socket, &tx_buf[0], tx_buf.size())) != tx_buf.size())
	{
		ERROR_MSG("Expected to transmit %d bytes, write returned %d", tx_sent);
		
		return false;
	}
	
	return true;
}

bool pivacy_ui_comm_thread::start()
{
	/* Only one instance of this thread should be running at any time */
	if (this->IsRunning())
	{
		return false;
	}
	
	this->Create();
	
	should_run = true;
	
	this->Run();
	
	return true;
}

void pivacy_ui_comm_thread::stop()
{
	/* Stop the thread and wait for it to exit */
	if (this->IsRunning())
	{
		should_run = false;
		
		this->Wait();
	}
}
