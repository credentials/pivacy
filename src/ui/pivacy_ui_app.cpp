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
 pivacy_ui_main.cpp

 The Pivacy UI main entry point
 *****************************************************************************/

#include "config.h"
#include "pivacy_ui_app.h"
#include "pivacy_ui_canvas.h"
#include "pivacy_config.h"
#include "pivacy_log.h"
#include <wx/cmdline.h>
#include <stdio.h>

IMPLEMENT_APP(pivacy_ui_app)

bool pivacy_ui_app::OnInit()
{
	::wxInitAllImageHandlers();
	
	/* Parse command-line parameters */
	wxCmdLineParser parser(this->argc, this->argv);
	
	parser.AddOption(wxT("c"), wxEmptyString, _("Use specified configuration file (defaults to ") + wxString(DEFAULT_PIVACY_UI_CONF, wxConvUTF8) + wxT(")"));
	parser.AddSwitch(wxT("h"), wxEmptyString, _("Print this help message"));
	parser.AddSwitch(wxT("v"), wxEmptyString, _("Print version information"));
	
	if (parser.Parse() != 0)
	{
		return false;
	}
	
	if (parser.Found(wxT("h")))
	{
		parser.Usage();
		
		return false;
	}
	
	if (parser.Found(wxT("v")))
	{
		printf("Pivacy User Interface daemon version %s\n", VERSION);
		printf("Copyright (c) 2013 Roland van Rijswijk-Deij\n\n");
		printf("Use, modification and redistribution of this software is subject to the terms\n");
		printf("of the license agreement. This software is licensed under a 2-clause BSD-style\n");
		printf("license a copy of which is included as the file LICENSE in the distribution.\n");
		
		return false;
	}
	
	std::string configfile = DEFAULT_PIVACY_UI_CONF;
	
	wxString configfile_option;
	
	if (parser.Found(wxT("c"), &configfile_option))
	{
		configfile = configfile_option.mb_str(wxConvUTF8);
	}
	
	/* Load configuration */
	if (pivacy_init_config_handling(configfile.c_str()) != PRV_OK)
	{
		fprintf(stderr, "Failed to load configuration from %s\n", configfile.c_str());
		
		return false;
	}
	
	/* Initialise logging */
	if (pivacy_init_log() != PRV_OK)
	{
		fprintf(stderr, "Failed to initialise logging, exiting\n");
		
		return false;
	}
	
	INFO_MSG("Pivacy UI version %s starting", VERSION);
	
	pivacy_ui_canvas* canvas = new pivacy_ui_canvas(wxSize(PIVACY_SCREENWIDTH, PIVACY_SCREENHEIGHT));
	canvas->Show(true);
	SetTopWindow(canvas);
	
	bool show_fullscreen = false;
	
	if (pivacy_conf_get_bool("ui", "fullscreen", show_fullscreen, true) != PRV_OK)
	{
		ERROR_MSG("Failed to read configuration option for fullscreen display");
		
		// Default to fullscreen
		canvas->to_fullscreen();
	}
	else
	{
		if (show_fullscreen)
		{
			canvas->to_fullscreen();
		}
	}
	
	canvas->set_status(_("Waiting for Pivacy system..."));
	
	return true;
}

int pivacy_ui_app::OnExit()
{
	/* Uninitialise logging */
	INFO_MSG("Pivacy UI version %s exiting", VERSION);
	
	pivacy_uninit_log();
	
	return 0;
}
