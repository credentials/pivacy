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
 pivacy_ui_pindialog.cpp

 The Pivacy UI PIN entry dialog
 *****************************************************************************/
 
#include "pivacy_ui_pindialog.h"
#include "pivacy_ui_colours.h"
#include <stdio.h>

pivacy_ui_pin_dialog::pivacy_ui_pin_dialog()
{
	areas_set = false;
	pressed = _("none");
	handling_event = false;
	pin_entry_evt = NULL;
}

void pivacy_ui_pin_dialog::render(wxGCDC& dc)
{
	dc.SetTextForeground(IRMA_DARK_BLUE);
	dc.SetTextBackground(IRMA_WHITE);
	dc.SetFont(wxFont(wxT("Ubuntu Bold 16")));
	
	dc.DrawText(_("Enter your PIN: "), 50, 50);
	
	// Render PIN entry field
	dc.SetPen(wxPen(IRMA_DARK_GREY, 3));
	dc.SetBrush(wxBrush(IRMA_LIGHT_GREY));
	dc.DrawRoundedRectangle(175, 48, 120, 22, 5);
	
	// Render stars for masked PIN
	wxString stars;
	
	for (int i = 0; i < pin_code.size(); i++)
	{
		stars += _("*");
	}
	
	wxCoord w,h;
	dc.GetTextExtent(stars, &w, &h);
	
	dc.SetTextForeground(IRMA_DARK_GREY);
	dc.SetTextBackground(IRMA_LIGHT_GREY);
	
	dc.DrawText(stars, 175 + 60 - (w / 2), 50);
	
	// Render the PIN pad
	dc.SetTextForeground(IRMA_WHITE);
	dc.SetTextBackground(IRMA_DARK_BLUE);
	dc.SetPen(wxPen(IRMA_DARK_BLUE));
	dc.SetBrush(IRMA_DARK_BLUE);
	
	wxCoord row = 80;
	wxCoord col[3] = { 0, 60 - 18, 120 - 36 };
	
	for (int i = 0; i < 12; i++)
	{
		if ((i != 0) && ((i % 3) == 0)) row += 36;
		
		if (i < 9)
		{
			wxString digit;
			digit.Printf(_("%d"), i + 1	);
			
			if (pressed == digit)
			{
				dc.SetPen(wxPen(IRMA_DARK_BLUE, 2));
				dc.SetBrush(IRMA_WHITE);
				dc.SetTextBackground(IRMA_WHITE);
				dc.SetTextForeground(IRMA_DARK_BLUE);
			}

			dc.DrawRoundedRectangle(175 + col[i % 3], row, 36, 30, 5);
			dc.DrawText(digit, 175 + col[i % 3] + 14, row + 6);
			
			if (pressed == digit)
			{
				dc.SetPen(wxPen(IRMA_DARK_BLUE));
				dc.SetBrush(IRMA_DARK_BLUE);
				dc.SetTextForeground(IRMA_WHITE);
				dc.SetTextBackground(IRMA_DARK_BLUE);
			}

			if (!areas_set)
			{
				areas.push_back(pivacy_ui_area(175 + col[i % 3], row, 36, 30, digit));
			}
		}
		else if (i == 9)
		{
			if (pressed == _("CLR"))
			{
				dc.SetPen(wxPen(IRMA_SIGNAL_RED, 2));
				dc.SetBrush(IRMA_WHITE);
				dc.SetTextBackground(IRMA_WHITE);
				dc.SetTextForeground(IRMA_SIGNAL_RED);
			}
			else
			{
				dc.SetTextForeground(IRMA_WHITE);
				dc.SetTextBackground(IRMA_SIGNAL_RED);
				dc.SetPen(wxPen(IRMA_SIGNAL_RED));
				dc.SetBrush(IRMA_SIGNAL_RED);
			}
			
			dc.DrawRoundedRectangle(175 + col[i % 3], row, 36, 30, 5);
			dc.DrawText(_("CLR"), 175 + col[i % 3] + 2, row + 6);
			
			if (!areas_set)
			{
				areas.push_back(pivacy_ui_area(175 + col[i % 3], row, 36, 30, _("CLR")));
			}
		}
		else if (i == 10)
		{
			if (pressed == _("0"))
			{
				dc.SetPen(wxPen(IRMA_DARK_BLUE, 2));
				dc.SetBrush(IRMA_WHITE);
				dc.SetTextBackground(IRMA_WHITE);
				dc.SetTextForeground(IRMA_DARK_BLUE);
			}
			else
			{
				dc.SetPen(wxPen(IRMA_DARK_BLUE));
				dc.SetBrush(IRMA_DARK_BLUE);
				dc.SetTextForeground(IRMA_WHITE);
				dc.SetTextBackground(IRMA_DARK_BLUE);
			}
			
			dc.DrawRoundedRectangle(175 + col[i % 3], row, 36, 30, 5);
			dc.DrawText(_("0"), 175 + col[i % 3] + 14, row + 6);
			
			if (!areas_set)
			{
				areas.push_back(pivacy_ui_area(175 + col[i % 3], row, 36, 30, _("0")));
			}
		}
		else if (i == 11)
		{
			if (pressed == _("OK"))
			{
				dc.SetPen(wxPen(IRMA_SIGNAL_GREEN, 2));
				dc.SetBrush(IRMA_WHITE);
				dc.SetTextBackground(IRMA_WHITE);
				dc.SetTextForeground(IRMA_SIGNAL_GREEN);
			}
			else
			{
				dc.SetTextForeground(IRMA_WHITE);
				dc.SetTextBackground(IRMA_SIGNAL_GREEN);
				dc.SetPen(wxPen(IRMA_SIGNAL_GREEN));
				dc.SetBrush(IRMA_SIGNAL_GREEN);
			}
			
			dc.DrawRoundedRectangle(175 + col[i % 3], row, 36, 30, 5);
			dc.DrawText(_("OK"), 175 + col[i % 3] + 6, row + 6);
			
			if (!areas_set)
			{
				areas.push_back(pivacy_ui_area(175 + col[i % 3], row, 36, 30, _("OK")));
			}
		}
	}
	
	areas_set = true;
}

bool pivacy_ui_pin_dialog::on_mouse(wxMouseEvent& event)
{
	bool rv = false;
	
	if (!areas_set)
	{
		return false;
	}
	
	if (event.LeftDown())
	{
		for (std::list<pivacy_ui_area>::iterator i = areas.begin(); i != areas.end(); i++)
		{
			if (i->in_area(event.GetX(), event.GetY()))
			{
				if (i->get_value() == _("CLR")	)
				{
					pin_code.clear();
				}
				else if (i->get_value() == _("OK"))
				{
					if (handling_event)
					{
						handling_event = false;
						
						pin_entry_evt->set_pin(pin_code);
						pin_entry_evt->signal_handled();
						pin_code.clear();
						pin_entry_evt = NULL;
					}
				}
				else if (i->get_value() != _("OK"))
				{
					if (pin_code.size() < 8)
					{
						pin_code += i->get_value().char_str();
					}
				}

				pressed = i->get_value();
				rv = true;
			}
		}
	}
	else if (event.LeftUp())
	{
		pressed = _("none");
		rv = true;
	}
	
	return rv;
}

void pivacy_ui_pin_dialog::handle_pin_entry(pivacy_ui_event& evt)
{
	pin_entry_evt = (pivacy_ui_event*) evt.Clone();
	handling_event = true;
	
	pin_code.clear();
}
