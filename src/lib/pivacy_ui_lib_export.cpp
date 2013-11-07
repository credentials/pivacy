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
 * GOODS OR SPRVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Pivacy
 * UI library exported functions
 */

#include "config.h"
#include "pivacy_ui_proto.h"
#include "pivacy_ui_lib.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

/* Library status */
static bool 	pivacy_ui_lib_initialised	= false;

static bool 	pivacy_ui_lib_connected		= false;

static bool		pivacy_ui_lib_must_cancel	= false;

/* Connection socket */
static int		pivacy_ui_socket			= -1;

pivacy_rv pivacy_ui_lib_init(void)
{
	if (pivacy_ui_lib_initialised)
	{
		return PRV_ALREADY_INITIALISED;
	}
	
	pivacy_ui_lib_connected = false;
	pivacy_ui_lib_must_cancel = false;
	pivacy_ui_socket = -1;
	
	pivacy_ui_lib_initialised = true;
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_lib_uninit(void)
{
	if (!pivacy_ui_lib_initialised)
	{
		return PRV_NOT_INITIALISED;
	}
	
	if (pivacy_ui_lib_connected)
	{
		pivacy_ui_disconnect();
	}
	
	pivacy_ui_lib_initialised = false;
	
	return PRV_OK;
}

int send_to_daemon(const std::vector<unsigned char> tx)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return -1;
	}
	
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
	 
	if ((tx_sent = write(pivacy_ui_socket, &tx_buf[0], tx_buf.size())) != tx_buf.size())
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_lib_connected = false;
		pivacy_ui_socket = -1;
		
		return -2;
	}
	
	return 0;
}

int recv_from_daemon(std::vector<unsigned char>& rx)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return -1;
	}
	
	unsigned short rx_size = 0;
	
	size_t rx_index = 0;
	
	unsigned char buf[512] = { 0 };
	
	/*
	 * Read the length of the data to receive
	 * 
	 * FIXME: this will fail if the process receives a signal!
	 */
	if ((read(pivacy_ui_socket, &buf[0], 1) != 1) ||
	    (read(pivacy_ui_socket, &buf[1], 1) != 1))
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_lib_connected = false;
		pivacy_ui_socket = -1;
		
		return -2;
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
		int received = read(pivacy_ui_socket, &buf[0], (rx_size < 512) ? rx_size : 512);
		
		if (received < 0)
		{
			close(pivacy_ui_socket);
		
			pivacy_ui_lib_connected = false;
			pivacy_ui_socket = -1;
			
			return -2;
		}
		
		memcpy(&rx[rx_index], buf, received);
		
		rx_index += received;
		rx_size -= received;
	}
	
	return 0;
}

pivacy_rv pivacy_ui_connect(void)
{
	if (pivacy_ui_lib_connected)
	{
		return PRV_ALREADY_CONNECTED;
	}
	
	/* Attempt to connect to the daemon */
	struct sockaddr_un addr = { 0 };
	
	pivacy_ui_socket = socket(PF_UNIX, SOCK_STREAM, 0);
	
	if (pivacy_ui_socket < 0)
	{
		pivacy_ui_socket = -1;
		
		return PRV_CONNECT_FAILED;
	}
	
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, UNIX_PATH_MAX, PIVACY_UI_SOCKET);
	
	if (connect(pivacy_ui_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) != 0)
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_socket = -1;
		
		return PRV_CONNECT_FAILED;
	}
	
	pivacy_ui_lib_connected = true;
	
	/* Request the API version from the daemon */
	std::vector<unsigned char> get_api_version;
	get_api_version.push_back(GET_API_VERSION);
	
	if (send_to_daemon(get_api_version) != 0)
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	std::vector<unsigned char> api_version_info;
	
	if (recv_from_daemon(api_version_info) != 0)
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	if ((api_version_info.size() != 1) || (api_version_info[0] != API_VERSION))
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_VERSION_MISMATCH;
	}
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_disconnect(void)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return PRV_NOT_CONNECTED;
	}
	
	/* Send disconnect command */
	std::vector<unsigned char> disconnect_cmd;
	disconnect_cmd.push_back(DISCONNECT);
	
	send_to_daemon(disconnect_cmd);
	
	close(pivacy_ui_socket);
	pivacy_ui_lib_connected = false;
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_transceive(std::vector<unsigned char>& cmd, std::vector<unsigned char>& resp)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return PRV_NOT_CONNECTED;
	}
	
	if (send_to_daemon(cmd) != 0)
	{
		close(pivacy_ui_socket);
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	if (recv_from_daemon(resp) != 0)
	{
		close(pivacy_ui_socket);
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_show_status(unsigned char status)
{
	std::vector<unsigned char> show_status_cmd;
	std::vector<unsigned char> show_status_resp;
	
	show_status_cmd.push_back(SHOW_STATUS);
	show_status_cmd.push_back(status);
	
	pivacy_rv rv;
	
	if ((rv = pivacy_ui_transceive(show_status_cmd, show_status_resp)) != PRV_OK)
	{
		return rv;
	}
	
	if ((show_status_resp.size() < 1) || (show_status_resp[0] != PIVACY_OK))
	{
		return PRV_PROTO_ERROR;
	}
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_request_pin(char* pin_buffer, size_t* pin_len)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return PRV_NOT_CONNECTED;
	}
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_consent(const char* rp_name, const char** attributes, size_t num_attrs, int show_always, int* consent_result)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return PRV_NOT_CONNECTED;
	}
	
	return PRV_OK;
}
