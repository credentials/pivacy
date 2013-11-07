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
 * Logging
 */

#ifndef _PIVACY_LOG_H
#define _PIVACY_LOG_H

#include "config.h"
#include "pivacy_ui_lib.h"

/* Log levels */
#define PIVACY_LOG_NONE		0
#define PIVACY_LOG_ERROR	1
#define PIVACY_LOG_WARNING	2
#define PIVACY_LOG_INFO		3
#define PIVACY_LOG_DEBUG	4

/* Initialise logging */
pivacy_rv pivacy_init_log(void);

/* Uninitialise logging */
pivacy_rv pivacy_uninit_log(void);

/* Log something */
void pivacy_log(const int log_at_level, const char* file, const int line, const char* format, ...);

/* Log directives */
#define ERROR_MSG(...) 		pivacy_log(PIVACY_LOG_ERROR  , __FILE__, __LINE__, __VA_ARGS__);
#define WARNING_MSG(...) 	pivacy_log(PIVACY_LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__);
#define INFO_MSG(...) 		pivacy_log(PIVACY_LOG_INFO   , __FILE__, __LINE__, __VA_ARGS__);
#define DEBUG_MSG(...) 		pivacy_log(PIVACY_LOG_DEBUG  , __FILE__, __LINE__, __VA_ARGS__);

#endif /* !_PIVACY_LOG_H */

