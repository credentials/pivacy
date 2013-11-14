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

int pivacy_ui_send_to_daemon(const std::vector<unsigned char> tx)
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

int pivacy_ui_recv_from_daemon(std::vector<unsigned char>& rx)
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
	
	if (pivacy_ui_send_to_daemon(get_api_version) != 0)
	{
		close(pivacy_ui_socket);
		
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	std::vector<unsigned char> api_version_info;
	
	if (pivacy_ui_recv_from_daemon(api_version_info) != 0)
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
	
	pivacy_ui_send_to_daemon(disconnect_cmd);
	
	close(pivacy_ui_socket);
	pivacy_ui_lib_connected = false;
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_transceive(std::vector<unsigned char>& cmd, std::vector<unsigned char>& resp, bool expectOK = true)
{
	if (!pivacy_ui_lib_connected || (pivacy_ui_socket < 0))
	{
		return PRV_NOT_CONNECTED;
	}
	
	if (pivacy_ui_send_to_daemon(cmd) != 0)
	{
		close(pivacy_ui_socket);
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	if (pivacy_ui_recv_from_daemon(resp) != 0)
	{
		close(pivacy_ui_socket);
		pivacy_ui_socket = -1;
		pivacy_ui_lib_connected = false;
		
		return PRV_DISCONNECTED;
	}
	
	if ((resp.size() < 1) || (resp[0] != PIVACY_OK))
	{
		return PRV_PROTO_ERROR;
	}
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_show_status(unsigned char status)
{
	std::vector<unsigned char> show_status_cmd;
	std::vector<unsigned char> show_status_resp;
	
	show_status_cmd.push_back(SHOW_STATUS);
	show_status_cmd.push_back(status);
	
	return pivacy_ui_transceive(show_status_cmd, show_status_resp);
}

pivacy_rv pivacy_ui_request_pin(char* pin_buffer, size_t* pin_len)
{
	if ((pin_buffer == NULL) || (pin_len == NULL))
	{
		return PRV_PARAM_INVALID;
	}
	
	std::vector<unsigned char> request_pin_cmd;
	std::vector<unsigned char> request_pin_rsp;
	
	request_pin_cmd.push_back(REQUEST_PIN);
	
	pivacy_rv rv;
	
	if ((rv = pivacy_ui_transceive(request_pin_cmd, request_pin_rsp)) != PRV_OK)
	{
		return rv;
	}
	
	if (*pin_len < (request_pin_rsp.size() - 1))
	{
		return PRV_BUFFER_TOO_SMALL;
	}
	
	*pin_len = request_pin_rsp.size() - 1;
	memcpy(pin_buffer, &request_pin_rsp[1], request_pin_rsp.size() - 1);
	
	return PRV_OK;
}

// WARNING: strlen(str) must be less than 256
void append_string_to_vector(std::vector<unsigned char>& vec, const char* str)
{
	vec.push_back((unsigned char) strlen(str));
	size_t pos = vec.size();
	vec.resize(vec.size() + strlen(str));
	memcpy(&vec[pos], str, strlen(str));
}

pivacy_rv pivacy_ui_consent(const char* rp_name, const char** attributes, size_t num_attrs, int show_always, int* consent_result)
{
	if ((rp_name == NULL) || ((attributes == NULL) && (num_attrs != 0)) || (consent_result == NULL) || (strlen(rp_name) > 255))
	{
		return PRV_PARAM_INVALID;
	}
	
	// Check attribute name lengths
	for (size_t i = 0; i < num_attrs; i++)
	{
		if (strlen(attributes[i]) > 255)
		{
			return PRV_PARAM_INVALID;
		}
	}
	
	std::vector<unsigned char> consent_cmd;
	std::vector<unsigned char> consent_rsp;
	
	consent_cmd.push_back(REQUEST_CONSENT);
	
	if (show_always)
	{
		consent_cmd.push_back(0x1);
	}
	else
	{
		consent_cmd.push_back(0x0);
	}
	
	append_string_to_vector(consent_cmd, rp_name);
	
	for (size_t i = 0; i < num_attrs; i++)
	{
		append_string_to_vector(consent_cmd, attributes[i]);
	}
	
	pivacy_rv rv;
	
	if ((rv = pivacy_ui_transceive(consent_cmd, consent_rsp)) != PRV_OK)
	{
		return rv;
	}
	
	if (consent_rsp.size() != 2)
	{
		return PRV_PROTO_ERROR;
	}
	
	*consent_result = consent_rsp[1];
	
	return PRV_OK;
}

pivacy_rv pivacy_ui_message(const char* msg)
{
	if (msg == NULL)
	{
		return PRV_PARAM_INVALID;
	}
	
	std::vector<unsigned char> show_msg_cmd;
	std::vector<unsigned char> show_msg_rsp;
	
	show_msg_cmd.resize(strlen(msg) + 1);
	
	show_msg_cmd[0] = SHOW_MESSAGE;
	memcpy(&show_msg_cmd[1], msg, strlen(msg));
	
	return pivacy_ui_transceive(show_msg_cmd, show_msg_rsp);
}
