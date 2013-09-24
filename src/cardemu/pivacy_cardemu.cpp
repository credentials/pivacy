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
 */

/*****************************************************************************
 pivacy_cardemu.cpp

 The IRMA card emulator main entry point
 *****************************************************************************/

#include "config.h"
#include "pivacy_config.h"
#include "pivacy_log.h"
#include "pivacy_errors.h"
#include "pivacy_cardemu_emulator.h"
#include "silvia_parameters.h"
#include "silvia_bytestring.h"
#include "edna.h"
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

/* Application AID for the IRMA card */
const static unsigned char AID[] = { 0x49, 0x52, 0x4D, 0x41, 0x63, 0x61, 0x72, 0x64 }; // "IRMAcard"

/* Global state */
static bool must_run = true;
static bool edna_connected = false;

static pivacy_cardemu_emulator* emulator = NULL;

void set_parameters()
{
	#define DEFAULT_L_N			1024
	#define DEFAULT_L_M			256
	#define DEFAULT_L_STATZK	80
	#define DEFAULT_L_H			256
	#define DEFAULT_L_V			1700
	#define DEFAULT_L_E			597
	#define DEFAULT_L_E_PRIME	120
	#define DEFAULT_HASH_TYPE	"sha256"
	
	////////////////////////////////////////////////////////////////////
	// Set the system parameters in the silvia library
	////////////////////////////////////////////////////////////////////
	
	int l_n, l_m, l_statzk, l_H, l_v, l_e, l_e_prime;
	std::string hash_type;
	
	pivacy_conf_get_int("emulation.idemix_parameters", "l_n", l_n, DEFAULT_L_N);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_m", l_m, DEFAULT_L_M);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_statzk", l_statzk, DEFAULT_L_STATZK);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_H", l_H, DEFAULT_L_H);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_v", l_v, DEFAULT_L_V);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_e", l_e, DEFAULT_L_E);
	pivacy_conf_get_int("emulation.idemix_parameters", "l_e_prime", l_e_prime, DEFAULT_L_E_PRIME);
	pivacy_conf_get_string("emulation.idemix_parameters", "hash_type", hash_type, DEFAULT_HASH_TYPE);
	
	INFO_MSG("The following Idemix system parameters will be used:");
	INFO_MSG("l(n)      = %d", l_n);
	INFO_MSG("l(m)      = %d", l_m);
	INFO_MSG("l(statzk) = %d", l_statzk);
	INFO_MSG("l(H)      = %d", l_H);
	INFO_MSG("l(v)      = %d", l_v);
	INFO_MSG("l(e)      = %d", l_e);
	INFO_MSG("l(e')     = %d", l_e_prime);
	INFO_MSG("hash type = %s", hash_type.c_str());
	
	silvia_system_parameters::i()->set_l_n(l_n);
	silvia_system_parameters::i()->set_l_m(l_m);
	silvia_system_parameters::i()->set_l_statzk(l_statzk);
	silvia_system_parameters::i()->set_l_H(l_H);
	silvia_system_parameters::i()->set_l_v(l_v);
	silvia_system_parameters::i()->set_l_e(l_e);
	silvia_system_parameters::i()->set_l_e_prime(l_e_prime);
	silvia_system_parameters::i()->set_hash_type(hash_type);
	silvia_system_parameters::i()->set_irma_kludge(true);
}

void write_pid(const char* pid_path, pid_t pid)
{
	FILE* pid_file = fopen(pid_path, "w");

	if (pid_file == NULL)
	{
		ERROR_MSG("Failed to write the pid file %s", pid_path);

		return;
	}

	fprintf(pid_file, "%d\n", pid);
	fclose(pid_file);
}

/* Signal handler for unexpected exit codes */
void signal_unexpected(int signum)
{
	switch(signum)
	{
	case SIGABRT:
		ERROR_MSG("Caught SIGABRT");
		break;
	case SIGBUS:
		ERROR_MSG("Caught SIGBUS");
		break;
	case SIGFPE:
		ERROR_MSG("Caught SIGFPE");
		break;
	case SIGILL:
		ERROR_MSG("Caught SIGILL");
		break;
	case SIGPIPE:
		ERROR_MSG("Caught SIGPIPE");
		break;
	case SIGQUIT:
		ERROR_MSG("Caught SIGQUIT");
		break;
	case SIGSEGV:
		ERROR_MSG("Caught SIGSEGV");
		exit(-1);
		break;
	case SIGSYS:
		ERROR_MSG("Caught SIGSYS");
		break;
	case SIGXCPU:
		ERROR_MSG("Caught SIGXCPU");
		break;
	case SIGXFSZ:
		ERROR_MSG("Caught SIGXFSZ");
		break;
	default:
		ERROR_MSG("Caught unknown signal 0x%X", signum);
		break;
	}
}

/* Signal handler for normal termination */
void signal_term(int signum)
{
	INFO_MSG("Received SIGINT or SIGTERM, exiting");
	
	if (edna_connected)
	{
		edna_lib_cancel();
	}
	
	must_run = false;
}

void version(void)
{
	printf("Pivacy IRMA card emulator version %s\n", VERSION);
	printf("\n");
	printf("Copyright (c) 2013 Roland van Rijswijk-Deij\n\n");
	printf("Use, modification and redistribution of this software is subject to the terms\n");
	printf("of the license agreement. This software is licensed under a 2-clause BSD-style\n");
	printf("license a copy of which is included as the file LICENSE in the distribution.\n");
}

void usage(void)
{
	printf("Pivacy IRMA card emulator version %s\n\n", VERSION);
	printf("Usage:\n");
	printf("\tpivacy_cardemu [-c <config-file>] [-p <pid-file>] [-f]");
	printf("\n");
	printf("\tpivacy_cardemu -h\n");
	printf("\tpivacy_cardemu -v\n");
	printf("\n");
	printf("\t-c <config-file> Read configuration from <config-file>\n");
	printf("\t                 (defaults to %s)\n", DEFAULT_PIVACY_CARDEMU_CONF);
	printf("\t-p <pid-file>    Write process ID to <pid-file>\n");
	printf("\t                 (defaults to %s)\n", DEFAULT_PIVACY_CARDEMU_PIDFILE);
	printf("\t-f               Run in the foreground rather than forking as a daemon\n");
	printf("\n");
	printf("\t-h                  Print this help message\n");
	printf("\n");
	printf("\t-v                  Print the version number\n");
}

int process_apdu(const unsigned char* apdu_data, size_t apdu_len, unsigned char* rdata, size_t* rdata_len)
{
	bytestring c_apdu(apdu_data, apdu_len);
	bytestring r_apdu = "6f00";
	
	if (emulator != NULL)
	{
		emulator->process_apdu(c_apdu, r_apdu);
		
		if (r_apdu.size() > *rdata_len) r_apdu = "6f00";
		
		memcpy(rdata, &r_apdu[0], r_apdu.size());
		
		*rdata_len = r_apdu.size();
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	// Program parameters
	std::string config_file = DEFAULT_PIVACY_CARDEMU_CONF;
	std::string pid_file = DEFAULT_PIVACY_CARDEMU_PIDFILE;
	bool daemon = true;
	bool daemon_set = false;
	bool pid_file_set = false;
	int c = 0;
	pid_t pid = 0;
	
	while ((c = getopt(argc, argv, "c:p:fhv")) != -1)
	{
		switch (c)
		{
		case 'h':
			usage();
			return 0;
		case 'v':
			version();
			return 0;
		case 'c':
			config_file = std::string(optarg);
			break;
		case 'p':
			pid_file = std::string(optarg);
			pid_file_set = true;
			break;
		case 'f':
			daemon = false;
			daemon_set = true;
			break;
		}
	}
	
	/* Load the configuration */
	if (pivacy_init_config_handling(config_file.c_str()) != PRV_OK)
	{
		fprintf(stderr, "Failed to load the configuration, exiting\n");

		return PRV_CONFIG_ERROR;
	}

	/* Initialise logging */
	if (pivacy_init_log() != PRV_OK)
	{
		fprintf(stderr, "Failed to initialise logging, exiting\n");

		return PRV_LOG_INIT_FAIL;
	}

	/* Determine configuration settings that were not specified on the command line */
	if (!pid_file_set)
	{
		std::string conf_pid_path;

		if (pivacy_conf_get_string("daemon", "pidfile", conf_pid_path, NULL) != PRV_OK)
		{
			ERROR_MSG("Failed to retrieve pidfile information from the configuration");
		}
		else
		{
			if (!conf_pid_path.empty())
			{
				pid_file = conf_pid_path;
			}
		}
	}

	if (!daemon_set)
	{
		if (pivacy_conf_get_bool("daemon", "fork", daemon, true) != PRV_OK)
		{
			ERROR_MSG("Failed to retrieve daemon information from the configuration");
		}
	}

	/* Now fork if that was requested */
	if (daemon)
	{
		pid = fork();

		if (pid != 0)
		{
			/* This is the parent process; write the PID file and exit */
			write_pid(pid_file.c_str(), pid);

			/* Unload the configuration */
			if (pivacy_uninit_config_handling() != PRV_OK)
			{
				ERROR_MSG("Failed to uninitialise configuration handling");
			}
		
			/* Uninitialise logging */
			if (pivacy_uninit_log() != PRV_OK)
			{
				fprintf(stderr, "Failed to uninitialise logging\n");
			}
		
			return PRV_OK;
		}
	}

	/* If we forked, this is the child */
	INFO_MSG("Starting pivacy IRMA card emulator version %s", VERSION);
	INFO_MSG("pivacy_cardemu %sprocess ID is %d", daemon ? "daemon " : "", getpid());

	/* Install signal handlers */
	signal(SIGABRT, signal_unexpected);
	signal(SIGBUS, signal_unexpected);
	signal(SIGFPE, signal_unexpected);
	signal(SIGILL, signal_unexpected);
	signal(SIGPIPE, signal_unexpected);
	signal(SIGQUIT, signal_unexpected);
	signal(SIGSEGV, signal_unexpected);
	signal(SIGSYS, signal_unexpected);
	signal(SIGXCPU, signal_unexpected);
	signal(SIGXFSZ, signal_unexpected);
	
	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);
	
	/* Set silvia system parameters */
	set_parameters();
	
	/* Set up emulator */
	emulator = new pivacy_cardemu_emulator();
	
	/* Initialise edna library */
	edna_rv rv = ERV_OK;
	
	if ((rv = edna_lib_init()) != ERV_OK)
	{
		ERROR_MSG("Failed to initialise the edna client library (0x%08X)", rv);
	}
	else
	{
		INFO_MSG("Initialised edna client library");
		
		must_run = true;
		
		while (must_run)
		{
			edna_connected = false;
			
			/* Try to connect to the edna daemon */
			while (must_run && (edna_lib_connect(AID, sizeof(AID)) != ERV_OK))
			{
				usleep(100000); // 100ms
			}
			
			if (!must_run) break;
			
			edna_connected = true;
			
			INFO_MSG("Connected to the edna daemon and registered IRMA card emulation");
			
			INFO_MSG("Starting APDU command handling");
			
			rv = edna_lib_loop_and_process(&process_apdu);
			
			if (rv != ERV_OK)
			{
				ERROR_MSG("APDU handling exited with an error (0x%08X)", rv);
			}
			
			edna_lib_disconnect();
			
			INFO_MSG("Disconnected from the edna daemon");
		}
		
		edna_lib_uninit();
		
		INFO_MSG("Uninitialised edna client library");
	}
	
	/* Clean up */
	//delete emulator;
	
	/* Tell the world we're exiting */
	INFO_MSG("The pivacy IRMA card emulator version %s has now stopped", VERSION);

	/* Unload the configuration */
	if (pivacy_uninit_config_handling() != PRV_OK)
	{
		ERROR_MSG("Failed to uninitialise configuration handling");
	}

	/* Remove signal handlers */
	signal(SIGABRT, SIG_DFL);
	signal(SIGBUS, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGSYS, SIG_DFL);
	signal(SIGXCPU, SIG_DFL);
	signal(SIGXFSZ, SIG_DFL);
	
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);

	/* Uninitialise logging */
	if (pivacy_uninit_log() != PRV_OK)
	{
		fprintf(stderr, "Failed to uninitialise logging\n");
	}

	return PRV_OK;
	
	return 0;
}
