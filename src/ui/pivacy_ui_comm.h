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

#ifndef _PIVACY_UI_COMM_H
#define _PIVACY_UI_COMM_H

#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h" 
#endif // WX_PRECOMP
#include <vector>
#include <string>

class pivacy_ui_event;

class pivacy_ui_comm_thread : public wxThread
{
public:
	/**
	 * Constructor
	 * @param main_wnd the main window to send events to
	 */
	pivacy_ui_comm_thread(wxWindow* main_wnd);

	/**
	 * Thread entry point
	 */
	virtual ExitCode Entry();
	
	/**
	 * Start the thread
	 */
	bool start();
	
	/**
	 * Stop the thread
	 */
	void stop();

private:
	/**
	 * Receive data from the client
	 * @param client_socket the socket to receive data from
	 * @param rx the data received
	 * @return true if the communication was successful, false otherwise
	 */
	bool recv_from_client(int client_socket, std::vector<unsigned char>& rx);
	
	/**
	 * Send data to the client
	 * @param client_socket the socket to send data to
	 * @param tx the data to send
	 * @return true if the data was sent successfully, false otherwise
	 */
	bool send_to_client(int client_socket, std::vector<unsigned char>& tx);
	
	/**
	 * Drop the client connection and restore the default UI state
	 * @param socket_fd the client socket to close
	 */
	void drop_client(int& socket_fd);
	
	/**
	 * Send the specified event to the main UI thread and wait for it to
	 * be handled
	 * @param evt the event to send
	 */
	void send_event_and_wait(pivacy_ui_event& evt);
	
	/**
	 * Retrieve a fixed length string from a byte array
	 * @param vec vector to retrieve the string from; the vector is altered
	 * @return the frontmost string in the byte array
	 */
	std::string string_from_vector(std::vector<unsigned char>& vec);

	// Should the thread be running
	bool should_run;
	
	// The main window
	wxWindow* main_wnd;
};

#endif // _PIVACY_UI_COMM_H
