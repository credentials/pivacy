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
 pivacy_ui_canvas.cpp

 The Pivacy UI main canvas
 *****************************************************************************/

#include "config.h"
#include "pivacy_ui_canvas.h"
#include "pivacy_ui_colours.h"
#include <wx/mstream.h>
#include <wx/dcbuffer.h>

////////////////////////////////////////////////////////////////////////
// Include images
////////////////////////////////////////////////////////////////////////

#include "irma_logo_75.inc"
#include "irma_logo_100.inc"
#include "irma_logo_125.inc"

////////////////////////////////////////////////////////////////////////
// Area class
////////////////////////////////////////////////////////////////////////

pivacy_ui_area::pivacy_ui_area(wxCoord left_top_x, wxCoord left_top_y, wxCoord w, wxCoord h, wxString value)
{
	this->left_top_x = left_top_x;
	this->left_top_y = left_top_y;
	this->w = w;
	this->h = h;
	this->value = value;
}

wxString pivacy_ui_area::get_value()
{
	return value;
}
	
bool pivacy_ui_area::in_area(wxCoord x, wxCoord y)
{
	if ((x >= left_top_x) && (x < (left_top_x + w)) &&
	    (y >= left_top_y) && (y < (left_top_y + h)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

////////////////////////////////////////////////////////////////////////
// Blank UX handler
////////////////////////////////////////////////////////////////////////

pivacy_ui_ux_blank::pivacy_ui_ux_blank()
{
	status = _("Starting up...");
}

void pivacy_ui_ux_blank::render(wxGCDC& dc)
{
	dc.SetTextForeground(IRMA_DARK_BLUE);
	dc.SetTextBackground(IRMA_WHITE);
	dc.SetFont(wxFont(_("Ubuntu Bold 16")));
	
	wxString version_info;
	version_info.Printf(_("Pivacy version %s"), _(VERSION));
	dc.DrawText(version_info, 50, 50);
	dc.DrawText(status, 50, 82);
}

bool pivacy_ui_ux_blank::on_mouse(wxMouseEvent& event)
{
}

void pivacy_ui_ux_blank::set_status(const wxString& status)
{
	this->status = status;
}

////////////////////////////////////////////////////////////////////////
// Canvas panel event table
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(pivacy_ui_canvas_panel, wxPanel)
    EVT_PAINT(pivacy_ui_canvas_panel::on_paint)
    EVT_MOUSE_EVENTS(pivacy_ui_canvas_panel::on_mouse)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////
// Canvas panel class implementation
////////////////////////////////////////////////////////////////////////

pivacy_ui_canvas_panel::pivacy_ui_canvas_panel(wxWindow* parent, const wxSize& size) :
	wxPanel(parent, wxID_ANY, wxPoint(0, 0), size)
{
	// Read in the IRMA logo
	wxMemoryInputStream irma_stream(irma_logo_75_png, sizeof(irma_logo_75_png));
	irma_logo = wxImage(irma_stream, wxBITMAP_TYPE_PNG);
	this->size = size;
	ux_handler = NULL;
}
	
void pivacy_ui_canvas_panel::on_paint(wxPaintEvent& event)
{
	wxGCDC dc(wxBufferedPaintDC(this));
	render(dc);
}

void pivacy_ui_canvas_panel::on_mouse(wxMouseEvent& event)
{
	if (ux_handler != NULL)
	{
		if (ux_handler->on_mouse(event))
		{
			repaint();
		}
	}
}
	
void pivacy_ui_canvas_panel::repaint()
{
	wxClientDC cdc(this);
	wxBufferedDC bdc(&cdc);
	wxGCDC dc(bdc);
	
	render(dc);
}
	
void pivacy_ui_canvas_panel::render(wxGCDC& dc)
{
	// Ensure that the background is "IRMA blue"
	dc.SetBrush(wxBrush(IRMA_DARK_BLUE));
	dc.SetPen(wxPen(IRMA_DARK_BLUE));
	wxCoord w, h;
	dc.GetSize(&w, &h);
	dc.DrawRectangle(0, 0, w, h);
	
	// Render the rounded bounding rectangle
	dc.SetPen(wxPen(IRMA_WHITE, 3));
	dc.SetBrush(wxBrush(IRMA_WHITE));
	dc.DrawRoundedRectangle(22, 22, size.GetWidth() - 30, size.GetHeight() - 30, 12);
	
	// Render the IRMA logo
	dc.DrawBitmap(wxBitmap(irma_logo), 0, 0);
	
	// Render the user interaction handler components
	if (ux_handler != NULL)
	{
		ux_handler->render(dc);
	}
}

void pivacy_ui_canvas_panel::set_ux_handler(pivacy_ui_ux_base* ux_handler)
{
	this->ux_handler = ux_handler;
	
	/*if (this->IsShown())
	{
		repaint();
	}*/
}

////////////////////////////////////////////////////////////////////////
// Canvas event table
////////////////////////////////////////////////////////////////////////

enum
{
	CANVAS_ID_QUIT = 1,
	CANVAS_ID_FULLSCREEN
};

BEGIN_EVENT_TABLE(pivacy_ui_canvas, wxFrame)
    EVT_MENU(CANVAS_ID_QUIT, pivacy_ui_canvas::on_quit)
    EVT_MENU(CANVAS_ID_FULLSCREEN, pivacy_ui_canvas::on_fullscreen)
    EVT_PAINT(pivacy_ui_canvas::on_paint)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////
// Canvas class implementation
////////////////////////////////////////////////////////////////////////

pivacy_ui_canvas::pivacy_ui_canvas(const wxSize& size) :
	wxFrame(NULL, -1, _("Pivacy"), wxDefaultPosition, size)
{
	wxMenu* file_menu = new wxMenu();
	
	file_menu->Append(CANVAS_ID_FULLSCREEN, _("Fullscreen\tCtrl-F"));
	file_menu->Append(CANVAS_ID_QUIT, _("Quit\tCtrl-Q"));
	
	wxMenuBar* menu = new wxMenuBar();
	menu->Append(file_menu, _("&File"));
	
	SetMenuBar(menu);
	
	// Construct the UI panel on which all the interaction will take place
	ui_panel = new pivacy_ui_canvas_panel(this, size);
	
	// Make the window lay out correctly
	wxGridSizer* sizer = new wxGridSizer(1, 0, 0);
	
	sizer->Add(ui_panel, 0, wxALIGN_CENTRE);
	
	SetSizerAndFit(sizer);
	
	ui_panel->set_ux_handler(&blank_ux_handler);
}

pivacy_ui_canvas::~pivacy_ui_canvas()
{
	delete ui_panel;
}

void pivacy_ui_canvas::on_quit(wxCommandEvent& event)
{
	Close(true);
}
	
void pivacy_ui_canvas::on_fullscreen(wxCommandEvent& event)
{
	ShowFullScreen(!IsFullScreen());
}

void pivacy_ui_canvas::on_paint(wxPaintEvent& event)
{
	// Ensure that the background of the window is in "IRMA blue"
	wxPaintDC dc(this);
	dc.SetBrush(wxBrush(IRMA_DARK_BLUE));
	dc.SetPen(wxPen(IRMA_DARK_BLUE));
	wxCoord w, h;
	dc.GetSize(&w, &h);
	dc.DrawRectangle(0, 0, w, h);
}

void pivacy_ui_canvas::set_ux_handler(pivacy_ui_ux_base* ux_handler)
{
	if (ux_handler == NULL)
	{
		// Show defined behaviour
		ui_panel->set_ux_handler(&blank_ux_handler);
	}
	else
	{
		ui_panel->set_ux_handler(ux_handler);
	}
}

void pivacy_ui_canvas::to_fullscreen()
{
	ShowFullScreen(true);
}

void pivacy_ui_canvas::set_status(const wxString& status)
{
	blank_ux_handler.set_status(status);
}
