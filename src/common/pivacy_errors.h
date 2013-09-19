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
 * Error conditions
 */

#ifndef _PIVACY_H
#define _PIVACY_H

#include <stdlib.h>

#define FLAG_SET(flags, flag) ((flags & flag) == flag)

/* Type for function return values */
typedef unsigned long pivacy_rv;

/* Function return values */

/* Success */
#define PRV_OK					0x00000000

/* Warning messages */

#define PRV_ALREADY_INITIALISED	0x40000000	/* The UI client library was already initialised */

/* Error messages */

/* General errors */
#define PRV_GENERAL_ERROR		0x80000000	/* An undefined error occurred */
#define PRV_MEMORY				0x80000001	/* An error occurred while allocating memory */
#define PRV_PARAM_INVALID		0x80000002	/* Invalid parameter(s) provided for function call */
#define PRV_LOG_INIT_FAIL		0x80000003	/* Failed to initialise logging */
#define PRV_NOT_INITIALISED		0x80000004	/* The client library is not initialised */

/* Configuration errors */
#define PRV_NO_CONFIG			0x80001000	/* No configuration file was specified */
#define PRV_CONFIG_ERROR		0x80001001	/* An error occurred while reading the configuration file */
#define PRV_CONFIG_NO_ARRAY		0x80001002	/* The requested configuration item is not an array */
#define PRV_CONFIG_NO_STRING	0x80001003	/* The requested configuration item is not a string */

/* Library errors */
#define PRV_CONNECT_FAILED		0x80002000	/* Failed to connect to the UI daemon */
#define PRV_DISCONNECTED		0x80002001	/* The connection with the UI daemon was closed unexpectedly */
#define PRV_NOT_CONNECTED		0x80002002	/* There is no connection to the UI daemon */
#define PRV_VERSION_MISMATCH	0x80002003	/* The UI daemon reported a mismatching API version */
#define PRV_ALREADY_CONNECTED	0x80002004	/* There is already a connection to the UI daemon */

#endif /* !_PIVACY_H */

