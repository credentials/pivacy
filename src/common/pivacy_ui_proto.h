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
 *
 */

/*
 * Pivacy
 * Protocol between the client library and the UI application
 */

#ifndef _PIVACY_UI_PROTO_H
#define _PIVACY_UI_PROTO_H

/* UNIX domain socket name */
#define PIVACY_UI_SOCKET	"/tmp/pivacy_ui-comm"

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 		80 			/* should be safe */
#endif // !UNIX_PATH_MAX

/* API version */
#define API_VERSION			0x00

/* API commands */
#define GET_API_VERSION		0x01
#define DISCONNECT			0x02
#define SHOW_STATUS			0x03
#define REQUEST_PIN			0x04
#define REQUEST_CONSENT		0x05
#define SHOW_MESSAGE		0x06

/* API return values */
#define PIVACY_OK			0x00
#define	PIVACY_UNKNOWN_CMD	0x01

#endif /* !_PIVACY_UI_PROTO_H */

