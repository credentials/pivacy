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
 * Configuration file handling
 */

#ifndef _PIVACY_CONFIG_H
#define _PIVACY_CONFIG_H

#include "config.h"
#include "pivacy_errors.h"
#include <string>

/* Initialise the configuration handler */
pivacy_rv pivacy_init_config_handling(const char* config_path);

/* Get an integer value */
pivacy_rv pivacy_conf_get_int(const char* base_path, const char* sub_path, int& value, int def_val);

/* Get a boolean value */
pivacy_rv pivacy_conf_get_bool(const char* base_path, const char* sub_path, bool& value, bool def_val);

/* Get a string value */
pivacy_rv pivacy_conf_get_string(const char* base_path, const char* sub_path, std::string& value, const char* def_val);

/* Release the configuration handler */
pivacy_rv pivacy_uninit_config_handling(void);

#endif /* !_PIVACY_CONFIG_H */

