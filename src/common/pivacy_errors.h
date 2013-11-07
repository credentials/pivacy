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

#ifndef _PIVACY_ERRORS_H
#define _PIVACY_ERRORS_H

#include <stdlib.h>
#include "pivacy_ui_lib.h"

#define FLAG_SET(flags, flag) ((flags & flag) == flag)

/* Internal error messages */

/* General errors */
#define PRV_LOG_INIT_FAIL		0x81000003	/* Failed to initialise logging */

/* Configuration errors */
#define PRV_NO_CONFIG			0x81001000	/* No configuration file was specified */
#define PRV_CONFIG_ERROR		0x81001001	/* An error occurred while reading the configuration file */
#define PRV_CONFIG_NO_ARRAY		0x81001002	/* The requested configuration item is not an array */
#define PRV_CONFIG_NO_STRING	0x81001003	/* The requested configuration item is not a string */

#endif /* !_PIVACY_ERRORS_H */

