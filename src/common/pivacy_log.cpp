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
 * Logging
 */

#include "config.h"
#include "pivacy_errors.h"
#include "pivacy_log.h"
#include "pivacy_config.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdlib.h>

/* The log level */
static int log_level = PIVACY_LOGLEVEL;

/* The log file */
static FILE* log_file = NULL;

/* Should we log to syslog? */
static bool log_syslog = 1;

/* Should we log to stdout? */
static bool log_stdout = 0;

/* Initialise logging */
pivacy_rv pivacy_init_log(void)
{
	std::string log_file_path;

	/* Retrieve the log level specified in the configuration file; this will override the default log level */
	if (pivacy_conf_get_int("log", "loglevel", log_level, PIVACY_LOGLEVEL) != PRV_OK)
	{
		return PRV_LOG_INIT_FAIL;
	}

	/* Retrieve the file name of the log file, if set */
	if (pivacy_conf_get_string("log", "logfile", log_file_path, NULL) != PRV_OK)
	{
		return PRV_LOG_INIT_FAIL;
	}

	if (!log_file_path.empty())
	{
		log_file = fopen(log_file_path.c_str(), "a");

		if (log_file == NULL)
		{
			fprintf(stderr, "Failed to open log file %s for writing\n", log_file_path.c_str());

			return PRV_LOG_INIT_FAIL;
		}
	}
	else
	{
		log_file = NULL;
	}

	/* Check whether we should log to syslog */
	if (pivacy_conf_get_bool("log", "syslog", log_syslog, 1) != PRV_OK)
	{
		return PRV_LOG_INIT_FAIL;
	}

	/* Check whether we should log to stdout */
	if (pivacy_conf_get_bool("log", "stdout", log_stdout, 0) != PRV_OK)
	{
		return PRV_LOG_INIT_FAIL;
	}

	return PRV_OK;
}

/* Uninitialise logging */
pivacy_rv pivacy_uninit_log(void)
{
	/* Close the log file if necessary */
	if (log_file != NULL)
	{
		fclose(log_file);
	}

	return PRV_OK;
}

/* Log something */
void pivacy_log(const int log_at_level, const char* file, const int line, const char* format, ...)
{
	static char log_buf[8192];
	static char timestamp[128];
	va_list args;

	/* Check the log level */
	if (log_at_level > log_level)
	{
		return;
	}

	/* Print the log message */
	va_start(args, format);

	if (log_at_level == PIVACY_LOG_DEBUG)
	{
		static char debug_buf[8192];
		vsnprintf(debug_buf, 8192, format, args);
		snprintf(log_buf, 8192, "%s(%d): %s", file, line, debug_buf);
	}
	else
	{
		vsnprintf(log_buf, 8192, format, args);
	}

	va_end(args);

	/* Check if we need to log to a file or stdout */
	if (log_stdout || log_file)
	{
		time_t now = 0;
		struct tm* now_tm = NULL;

		/* Create a timestamp */
		now = time(NULL);
		now_tm = localtime(&now);

		snprintf(timestamp, 128, "%4d-%02d-%02d %02d:%02d:%02d",
			now_tm->tm_year+1900,
			now_tm->tm_mon+1,
			now_tm->tm_mday,
			now_tm->tm_hour,
			now_tm->tm_min,
			now_tm->tm_sec);

		if (log_stdout)
		{
			printf("%s %s\n", timestamp, log_buf);
			fflush(stdout);
		}

		if (log_file)
		{
			fprintf(log_file, "%s %s\n", timestamp, log_buf);
			fflush(log_file);
		}
	}

	/* Check if we need to log to syslog */
	if (log_syslog)
	{
		syslog(log_at_level, "%s", log_buf);
	}
}

