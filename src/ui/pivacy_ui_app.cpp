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
#include "pivacy_ui_pindialog.h"
#include "pivacy_ui_consent.h"
#include "pivacy_ui_status.h"

pivacy_ui_pin_dialog* pin_dialog;
pivacy_ui_consent_dialog* consent_dialog;
pivacy_ui_status_dialog* status_dialog;

IMPLEMENT_APP(pivacy_ui_app)

bool pivacy_ui_app::OnInit()
{
	::wxInitAllImageHandlers();
	
	pivacy_ui_canvas* canvas = new pivacy_ui_canvas(wxSize(PIVACY_SCREENWIDTH, PIVACY_SCREENHEIGHT));
	canvas->Show(true);
	SetTopWindow(canvas);
	
	pin_dialog = new pivacy_ui_pin_dialog();

	consent_dialog = new pivacy_ui_consent_dialog();
	std::list<wxString> attr;
	attr.push_back(_("Over 18"));
	attr.push_back(_("Over 21"));
	consent_dialog->set_rp_and_attr(_("Albron Catering"), attr);
	consent_dialog->set_show_always(false);
	
	status_dialog = new pivacy_ui_status_dialog(pivacy_ui_status_dialog::PIVACY_STATUS_PRESENT);
	
	//canvas->set_ux_handler(pin_dialog);
	canvas->set_ux_handler(consent_dialog);
	//canvas->set_ux_handler(status_dialog);

	canvas->to_fullscreen();
	
	canvas->set_status(_("Waiting for IRMA system..."));
	
	return true;
}
