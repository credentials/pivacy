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
 * Pivacy UI
 * UI client sample code
 */

#include "config.h"
#include "pivacy_ui_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
	pivacy_rv rv = PRV_OK;
	
	/* Initialise the library */
	if (pivacy_ui_lib_init() != PRV_OK)
	{
		fprintf(stderr, "Failed to initialise the Pivacy UI library\n");
		
		return -1;
	}
	
	/* Connect to the daemon */
	if ((rv = pivacy_ui_connect()) != PRV_OK)
	{
		fprintf(stderr, "Failed to connect to the pivacy_ui daemon (0x%08X)\n", (unsigned int) rv);
		
		return -1;
	}
	
	/* Try all the "show status" commands */
	for (int i = 1; i <= 5; i++)
	{
		if ((rv = pivacy_ui_show_status(i)) != PRV_OK)
		{
			fprintf(stderr, "Failed to send SHOW STATUS command (0x%08X)\n", (unsigned int) rv);
			
			return -1;
		}
		sleep(1);
	}
	
	/* Try the "show message" command */
	if ((rv = pivacy_ui_message("Waiting for edna daemon to start...")) != PRV_OK)
	{
		fprintf(stderr, "Failed to send SHOW MESSAGE command (0x%08X)\n", (unsigned int) rv);
		
		return -1;
	}
	sleep(1);
	
	/* Try the "request PIN" command */
	char pin[9] = { 0 };
	size_t pin_len = 8;
	
	if ((rv = pivacy_ui_request_pin(pin, &pin_len)) != PRV_OK)
	{
		fprintf(stderr, "Failed to send REQUEST PIN command (0x%08X)\n", (unsigned int) rv);
		
		return -1;
	}
	
	printf("User entered the following PIN: %s (%zd)\n", pin, pin_len);
	
	/* Try the "request consent" command */
	const char* attrs[] =
	{
		"Over 18",
		"Nationality"
	};
	int consent_result;
	
	if ((rv = pivacy_ui_consent("Coffeeshop Weedrook", attrs, 2, 1, &consent_result)) != PRV_OK)
	{
		fprintf(stderr, "Failed to send GET CONSENT command (0x%08X)\n", (unsigned int) rv);
		
		return -1;
	}
	
	printf("User consent answer was: ");
	
	switch(consent_result)
	{
	case PIVACY_CONSENT_ONCE:
		printf("ONCE\n");
		break;
	case PIVACY_CONSENT_ALWAYS:
		printf("ALWAYS\n");
		break;
	case PIVACY_CONSENT_NO:
		printf("REFUSE\n");
		break;
	default:
		printf("UNKNOWN\n");
		break;
	}
	
	if ((rv = pivacy_ui_consent("Coffeeshop Weedrook", attrs, 2, 0, &consent_result)) != PRV_OK)
	{
		fprintf(stderr, "Failed to send GET CONSENT command (0x%08X)\n", (unsigned int) rv);
		
		return -1;
	}
	
	printf("User consent answer was: ");
	
	switch(consent_result)
	{
	case PIVACY_CONSENT_ONCE:
		printf("ONCE\n");
		break;
	case PIVACY_CONSENT_ALWAYS:
		printf("ALWAYS\n");
		break;
	case PIVACY_CONSENT_NO:
		printf("REFUSE\n");
		break;
	default:
		printf("UNKNOWN\n");
		break;
	}
	
	/* Disconnect from the daemon */
	pivacy_ui_disconnect();
	
	/* Uninitialise the library */
	pivacy_ui_lib_uninit();
	
	return 0;
}

