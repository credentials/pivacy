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
 * Configuration file handling
 */

#include "config.h"
#include "pivacy_config.h"
#include <libconfig.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

/* The configuration */
config_t configuration;

/* Initialise the configuration handler */
pivacy_rv pivacy_init_config_handling(const char* config_path)
{
	if ((config_path == NULL) || (strlen(config_path) == 0))
	{
		return PRV_NO_CONFIG;
	}

	/* Initialise the configuration */
	config_init(&configuration);

	/* Load the configuration from the specified file */
	if (config_read_file(&configuration, config_path) != CONFIG_TRUE)
	{
		fprintf(stderr, "Failed to read the configuration: %s (%s:%d)\n",
			config_error_text(&configuration),
			config_path,
			config_error_line(&configuration));

		config_destroy(&configuration);

		return PRV_CONFIG_ERROR;
	}

	return PRV_OK;
}

/* Release the configuration handler */
pivacy_rv pivacy_uninit_config_handling(void)
{
	/* Uninitialise the configuration */
	config_destroy(&configuration);

	return PRV_OK;
}

/* Get an integer value */
pivacy_rv pivacy_conf_get_int(const char* base_path, const char* sub_path, int& value, int def_val)
{
	/* Unfortunately, the kludge below is necessary since the interface for config_lookup_int changed between
	 * libconfig version 1.3 and 1.4 */
#ifndef LIBCONFIG_VER_MAJOR /* this means it is a pre 1.4 version */
	long conf_val = 0;
#else
	int conf_val = 0;
#endif /* libconfig API kludge */
	static char path_buf[8192];

	if ((base_path == NULL) || (sub_path == NULL))
	{
		return PRV_PARAM_INVALID;
	}

	snprintf(path_buf, 8192, "%s.%s", base_path, sub_path);

	if (config_lookup_int(&configuration, path_buf, &conf_val) != CONFIG_TRUE)
	{
		value = def_val;
	}
	else
	{
		value = conf_val;
	}

	return PRV_OK;
}

/* Get a boolean value */
pivacy_rv pivacy_conf_get_bool(const char* base_path, const char* sub_path, bool& value, bool def_val)
{
	int conf_val = 0;
	static char path_buf[8192];

	if ((base_path == NULL) || (sub_path == NULL))
	{
		return PRV_PARAM_INVALID;
	}

	snprintf(path_buf, 8192, "%s.%s", base_path, sub_path);

	if (config_lookup_bool(&configuration, path_buf, &conf_val) != CONFIG_TRUE)
	{
		value = def_val;
	}
	else
	{
		value = (conf_val == CONFIG_TRUE) ? true : false;
	}

	return PRV_OK;
}

/* Get a string value */
pivacy_rv pivacy_conf_get_string(const char* base_path, const char* sub_path, std::string& value, const char* def_val)
{
	const char* conf_val = NULL;
	static char path_buf[8192];

	if ((base_path == NULL) || (sub_path == NULL))
	{
		return PRV_PARAM_INVALID;
	}

	snprintf(path_buf, 8192, "%s.%s", base_path, sub_path);

	if (config_lookup_string(&configuration, path_buf, &conf_val) != CONFIG_TRUE)
	{
		if (def_val != NULL)
		{
			value = std::string(def_val);
		}
		else
		{
			value.clear();
		}
	}
	else
	{
		value = std::string(conf_val);
	}

	return PRV_OK;
}
