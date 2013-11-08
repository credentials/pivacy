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
 pivacy_ui_lib.h

 The Pivacy UI library interface
 *****************************************************************************/
 
#ifndef _PIVACY_UI_LIB_H
#define _PIVACY_UI_LIB_H

#include <stdlib.h>
 
/* Return value type */
typedef unsigned long pivacy_rv;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/**
 * Initialise the Pivacy UI client library
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_lib_init(void);

/**
 * Uninitialise the Pivacy UI client library
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_lib_uninit(void);

/**
 * Connect to the Pivacy UI
 * @return PRV_OK if successful, in case of errors either PRV_CONNECT_FAILED,
 * PRV_VERSION_MISMATCH, PRV_ALREADY_CONNECTED or PRV_CONNECTION_DENIED
 */
pivacy_rv pivacy_ui_connect(void);

/**
 * Disconnect from the Pivacy UI
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_disconnect(void);

#define PIVACY_STATE_WAIT		1			/* Pivacy is waiting for a card terminal to connect */
#define PIVACY_STATE_PRESENT	2			/* Pivacy has detected a card terminal */
#define PIVACY_STATE_OK			3			/* The Pivacy interaction was successful */
#define PIVACY_STATE_WARN		4			/* A communication error occurred */
#define PIVACY_STATE_FAIL		5			/* The Pivacy interaction failed */

/**
 * Switch to the status display for the specified state
 * @param status the status to report
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_show_status(unsigned char status);

#define MAX_PIN_LEN				8

/**
 * Request the user to enter their PIN
 * @param pin_buffer a buffer that will receive the PIN; must be MAX_PIN_LEN bytes in size
 * @param pin_len on input, the size of the PIN buffer, on output the length of the actual PIN entered
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_request_pin(char* pin_buffer, size_t* pin_len);

#define PIVACY_CONSENT_NO		1			/* User did not give consent */
#define PIVACY_CONSENT_ONCE		2			/* User gave consent once */
#define PIVACY_CONSENT_ALWAYS	3			/* User provides consent for all time */

/**
 * Request user consent
 * @param rp_name the name of the relying party (NULL-terminated string)
 * @param attributes the names of the attributes that are to be revealed (array of NULL-terminated strings)
 * @param num_attrs the number of attributes
 * @param show_always set to a value other than 0 if the ALWAYS button should be displayed
 * @param consent_result the consent decision taken by the user
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_consent(const char* rp_name, const char** attributes, size_t num_attrs, int show_always, int* consent_result);

/**
 * Show a message to the user
 * @param msg the message to display
 * @return PRV_OK if successful
 */
pivacy_rv pivacy_ui_message(const char* msg);

#ifdef __cplusplus
}
#endif // __cplusplus

/* Return values */
/* Success */
#define PRV_OK					0x00000000

/* Warning messages */
#define PRV_ALREADY_INITIALISED	0x40000000	/* The client library was already initialised */

/* Error messages */

/* General errors */
#define PRV_GENERAL_ERROR		0x80000000	/* An undefined error occurred */
#define PRV_MEMORY				0x80000001	/* An error occurred while allocating memory */
#define PRV_PARAM_INVALID		0x80000002	/* Invalid parameter(s) provided for function call */
#define PRV_NOT_INITIALISED		0x80000003	/* The client library is not initialised */

/* Library errors */
#define PRV_CONNECT_FAILED		0x80002000	/* Failed to connect to the daemon */
#define PRV_DISCONNECTED		0x80002001	/* The connection with the daemon was closed unexpectedly */
#define PRV_NOT_CONNECTED		0x80002002	/* There is no connection to the daemon */
#define PRV_VERSION_MISMATCH	0x80002003	/* The daemon reported a mismatching API version */
#define PRV_ALREADY_CONNECTED	0x80002004	/* There is already a connection to the daemon */
#define PRV_CONNECTION_DENIED	0x80002005	/* The connection was denied because another client is already using the UI */
#define PRV_PROTO_ERROR			0x80002006	/* Protocol error */
#define PRV_BUFFER_TOO_SMALL	0x80002007	/* The provided buffer is too small */

#endif // !_PIVACY_UI_LIB_H
