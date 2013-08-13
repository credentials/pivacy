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
 pivacy_ui_canvas.h

 The Pivacy UI main canvas
 *****************************************************************************/
 
#ifndef _PIVACY_UI_CANVAS_H
#define _PIVACY_UI_CANVAS_H
 
#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h" 
#endif // WX_PRECOMP

/**
 * Screen area class
 */
 
class pivacy_ui_area
{
public:
	/**
	 * Constructor
	 * @param left_top_x x position of left-hand top corner
	 * @param left_top_y y position of left-hand top corner
	 * @param w width
	 * @param h height
	 * @param value value associated with this area
	 */
	pivacy_ui_area(wxCoord left_top_x, wxCoord left_top_y, wxCoord w, wxCoord h, wxString value);
	
	/**
	 * Get the value
	 * @return the value associated with the area
	 */
	wxString get_value();
	
	/**
	 * Is the given point in the area?
	 * @param x the x position of the point to test
	 * @param y the y position of the point to test
	 * @return true if (x,y) is in the area, false otherwise
	 */
	bool in_area(wxCoord x, wxCoord y);
	
private:
	wxCoord left_top_x;
	wxCoord left_top_y;
	wxCoord w;
	wxCoord h;
	wxString value;
};

/**
 * UI interaction base class
 */
 
class pivacy_ui_ux_base
{
public:
	/**
	 * Paint the user interface elements
	 * @param dc the device context to render on
	 */
	virtual void render(wxGCDC& dc) = 0;

	/**
	 * Handle mouse events
	 * @param event the mouse event
	 * @return true if the parent window should be repainted
	 */
	virtual bool on_mouse(wxMouseEvent& event) = 0;
};

/**
 * Blank UI used during startup
 */
 
class pivacy_ui_ux_blank : public pivacy_ui_ux_base
{
public:
	/**
	 * Constructor
	 */
	pivacy_ui_ux_blank();
	
	/**
	 * Paint the user interface elements
	 * @param dc the device context to render on
	 */
	virtual void render(wxGCDC& dc);

	/**
	 * Handle mouse events
	 * @param event the mouse event
	 * @return true if the parent window should be repainted
	 */
	virtual bool on_mouse(wxMouseEvent& event);
	
	/**
	 * Set the status string
	 * @param status the current system status
	 */
	void set_status(const wxString& status);

private:
	wxString status;
};

/**
 * Canvas panel
 */

class pivacy_ui_canvas_panel : public wxPanel
{
public:
	/**
	 * Constructor
	 * @param parent the parent window
	 * @param size the panel size
	 */
	pivacy_ui_canvas_panel(wxWindow* parent, const wxSize& size);
	
	/**
	 * Destructor
	 */
	virtual ~pivacy_ui_canvas_panel() { }
	
	/**
	 * Paint event handler
	 * @param event paint event
	 */
	void on_paint(wxPaintEvent& event);

	/**
	 * Mouse event handler
	 * @param event the mouse event
	 */
	void on_mouse(wxMouseEvent& event);
	
	/**
	 * Repaint the panel now
	 */
	void repaint();
	
	/**
	 * Render the panel contents
	 * @param dc the device context
	 */
	void render(wxGCDC& dc);
	
	/**
	 * Set the user interaction handler
	 * @param ux_handler the user interaction handler
	 */
	void set_ux_handler(pivacy_ui_ux_base* ux_handler);
	
	DECLARE_EVENT_TABLE()

private:
	wxImage irma_logo;
	wxSize size;
	pivacy_ui_ux_base* ux_handler;
};

/**
 * Main canvas class
 */

class pivacy_ui_canvas : public wxFrame
{
public:
	/**
	 * Constructor
	 * @param run_in_window should the application run in a window (defaults to false)
	 */
	pivacy_ui_canvas(const wxSize& size);
	
	/**
	 * Destructor
	 */
	~pivacy_ui_canvas();
	
	/**
	 * Quit handler
	 * @param event quit event
	 */
	void on_quit(wxCommandEvent& event);
	
	/**
	 * Fullscreen handler
	 * @param event fullscreen event
	 */
	void on_fullscreen(wxCommandEvent& event);
	
	/**
	 * Paint event handler
	 * @param event paint event
	 */
	void on_paint(wxPaintEvent& event);
	
	/**
	 * Set the UX handler
	 * @param ux_handler the UX handler
	 */
	void set_ux_handler(pivacy_ui_ux_base* ux_handler);

	/**
	 * Switch to full screen mode
	 */
	void to_fullscreen();
	
	/**
	 * Set the system status (displayed on the blank UX panel)
	 */
	void set_status(const wxString& status);
	
	DECLARE_EVENT_TABLE()

private:
	// The rendering panel
	pivacy_ui_canvas_panel* ui_panel;
	
	// Basic UX handler shown when no other has been specified
	pivacy_ui_ux_blank blank_ux_handler;
};

#endif // !_PIVACY_UI_CANVAS_H
