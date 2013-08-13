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
 pivacy_ui_consent.cpp

 The Pivacy UI consent dialog
 *****************************************************************************/

#include "config.h"
#include "pivacy_ui_consent.h"
#include "pivacy_ui_colours.h"
#include <stdio.h>

pivacy_ui_consent_dialog::pivacy_ui_consent_dialog(wxString rp, std::list<wxString> attr)
{
	areas_set = false;
	this->rp = rp;
	this->attr = attr;
}

void pivacy_ui_consent_dialog::render(wxGCDC& dc)
{
	dc.SetTextForeground(IRMA_DARK_BLUE);
	dc.SetTextBackground(IRMA_WHITE);
	dc.SetFont(wxFont(_("Ubuntu Bold 16")));

	// Render the name of the relying party
	wxCoord w,h;
	dc.GetTextExtent(rp, &w, &h);
	
	dc.DrawText(rp, 22 + (((PIVACY_SCREENWIDTH - 30) - w) / 2), 50);

	// Render the request
	dc.SetFont(wxFont(_("Ubuntu Bold 14")));
	dc.DrawText(_("Requests the following information:"), 30, 80);

	wxCoord row = 80 + 15 + 15;

	for (std::list<wxString>::iterator i = attr.begin(); i != attr.end(); i++)
	{
		dc.DrawText(*i, 50, row);

		row += 15;
	}

	// Render the consent buttons
	dc.SetFont(wxFont(_("Ubuntu Bold 16")));
	wxCoord consent_width, always_width, refuse_width;
	dc.GetTextExtent(_("CONSENT"), &consent_width, &h);
	dc.GetTextExtent(_("ALWAYS"), &always_width, &h);
	dc.GetTextExtent(_("REFUSE"), &refuse_width, &h);

	consent_width += 12;
	always_width += 12;
	refuse_width += 12;

	wxCoord left_x = 22 + (((PIVACY_SCREENWIDTH - 30) - (consent_width + 10 + always_width + 10 + refuse_width)) / 2);

	wxCoord consent_x = left_x;
	wxCoord always_x = left_x + consent_width + 10;
	wxCoord refuse_x = left_x + consent_width + 10 + always_width + 10;
	wxCoord button_y = PIVACY_SCREENHEIGHT - 8 - 10 - 30;
	
	dc.SetTextForeground(IRMA_WHITE);
	dc.SetTextBackground(IRMA_DARK_BLUE);
	dc.SetPen(wxPen(IRMA_DARK_BLUE));
	dc.SetBrush(IRMA_DARK_BLUE);

	dc.DrawRoundedRectangle(consent_x, button_y, consent_width, 30, 5);
	dc.DrawText(_("CONSENT"), consent_x + 6, button_y + 6);

	dc.DrawRoundedRectangle(always_x, button_y, always_width, 30, 5);
	dc.DrawText(_("ALWAYS"), always_x + 6, button_y + 6);

	dc.SetPen(wxPen(IRMA_SIGNAL_RED));
	dc.SetBrush(IRMA_SIGNAL_RED);
	dc.SetTextForeground(IRMA_WHITE);
	dc.SetTextBackground(IRMA_SIGNAL_RED);

	dc.DrawRoundedRectangle(refuse_x, button_y, refuse_width, 30, 5);
	dc.DrawText(_("REFUSE"), refuse_x + 6, button_y + 6);

	if (!areas_set)
	{
		areas.push_back(pivacy_ui_area(consent_x, button_y, consent_width, 30, _("CONSENT")));
		areas.push_back(pivacy_ui_area(always_x, button_y, always_width, 30, _("ALWAYS")));
		areas.push_back(pivacy_ui_area(refuse_x, button_y, refuse_width, 30, _("REFUSE")));
	}

	areas_set = true;
}

bool pivacy_ui_consent_dialog::on_mouse(wxMouseEvent& event)
{
	bool rv = false;

	if (event.LeftDown())
	{
		for (std::list<pivacy_ui_area>::iterator i = areas.begin(); i != areas.end(); i++)
		{
			if (i->in_area(event.GetX(), event.GetY()))
			{
				if (i->get_value() == _("CONSENT"))
				{
					printf("User pressed CONSENT\n");
				}
				else if (i->get_value() == _("ALWAYS"))
				{
					printf("User pressed ALWAYS\n");
				}
				else if (i->get_value() == _("REFUSE"))
				{
					printf("User pressed REFUSE\n");
				}
			}
		}
	}
	
	return rv;
}
