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
 pivacy_ui_status.cpp

 The Pivacy UI consent dialog
 *****************************************************************************/

#include "config.h"
#include "pivacy_ui_status.h"
#include "pivacy_ui_colours.h"
#include <stdio.h>
#include <wx/mstream.h>

#include "images/ok_160.inc"
#include "images/warn_160.inc"
#include "images/fail_160.inc"
#include "images/wait_160.inc"
#include "images/present_160.inc"

pivacy_ui_status_dialog::pivacy_ui_status_dialog(int status)
{
	switch(status)
	{
	case PIVACY_STATUS_OK:
		{
			wxMemoryInputStream status_image_stream(ok_160_png, sizeof(ok_160_png));
			status_image = wxImage(status_image_stream, wxBITMAP_TYPE_PNG);
		}
		break;
	case PIVACY_STATUS_WARN:
		{
			wxMemoryInputStream status_image_stream(warn_160_png, sizeof(warn_160_png));
			status_image = wxImage(status_image_stream, wxBITMAP_TYPE_PNG);
		}
		break;
	case PIVACY_STATUS_FAIL:
		{
			wxMemoryInputStream status_image_stream(fail_160_png, sizeof(fail_160_png));
			status_image = wxImage(status_image_stream, wxBITMAP_TYPE_PNG);
		}
		break;
	case PIVACY_STATUS_WAIT:
		{
			wxMemoryInputStream status_image_stream(wait_160_png, sizeof(wait_160_png));
			status_image = wxImage(status_image_stream, wxBITMAP_TYPE_PNG);
		}
		break;
	case PIVACY_STATUS_PRESENT:
		{
			wxMemoryInputStream status_image_stream(present_160_png, sizeof(present_160_png));
			status_image = wxImage(status_image_stream, wxBITMAP_TYPE_PNG);
		}
		break;
	default:
		return;
	}
}

void pivacy_ui_status_dialog::render(wxGCDC& dc)
{
	static wxCoord x = ((PIVACY_SCREENWIDTH - 30) / 2) - 80 + 22;
	static wxCoord y = ((PIVACY_SCREENHEIGHT - 30) / 2) - 80 + 22;
	
	dc.DrawBitmap(wxBitmap(status_image), x, y);
}

bool pivacy_ui_status_dialog::on_mouse(wxMouseEvent& event)
{
	return false;
}
