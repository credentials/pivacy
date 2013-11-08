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
#include "pivacy_ui_comm.h"
#include <string>

/*
 * Pre-define classes
 */
 
class pivacy_ui_pin_dialog;
class pivacy_ui_status_dialog;
class pivacy_ui_consent_dialog;

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
 * Event class for communication between comm thread and main canvas
 */
 
#define PEVT_SHOWSTATUS				0x1
#define PEVT_REQUESTPIN				0x2
#define PEVT_REQUESTCONSENT			0x3
#define PEVT_NOCLIENT				0x4
#define PEVT_SHOWMSG				0x5

class pivacy_ui_event_data
{
public:
	// Event return data
	std::string PIN;
	int consent_result;
};

class pivacy_ui_event : public wxEvent
{
public:
	/**
	 * Blank constructor
	 */
	pivacy_ui_event();

	/**
	 * Constructor
	 * @param type the event type
	 * @param win the associated window
	 */
	pivacy_ui_event(int type, pivacy_ui_event_data* evt_data = NULL, wxWindow* win = NULL);
	
	/**
	 * Set the waiting mutex and condition
	 * @param wait_mutex mutex used to wait until the event has been handled by the main thread
	 * @param wait_cond condition used to wait until the event has been handled by the main thread
	 */
	void set_mutex_and_cond(wxMutex* wait_mutex, wxCondition* wait_cond);
	
	/**
	 * Set the status to show (in case of PEVT_SHOWSTATUS event)
	 * @param status the status to show
	 */
	void set_show_status(int status);
	
	/**
	 * Get the status to show
	 * @return the status to show
	 */
	int get_show_status();
	
	/**
	 * Set the message to show (in case of PEVT_SHOWMESSAGE event)
	 * @param msg the message to show
	 */
	void set_show_message(std::string& msg);
	
	/**
	 * Get the message to show
	 * @return the message to show
	 */
	wxString get_show_message();
	
	/**
	 * Set the PIN
	 * @param PIN the PIN entered by the user
	 */
	void set_pin(std::string PIN);
	
	/**
	 * Set the relying party that is asking consent
	 * @param rp_name the name of the relying party
	 */
	void set_rp_name(wxString& rp_name);
	
	/**
	 * Set the names of the attributes that the relying party is asking for
	 * @param attributes the attributes
	 */
	void set_rp_attributes(std::vector<wxString> attributes);
	
	/**
	 * Get the relying party that is asking for consent
	 * @return the name of the relying party that is asking for consent
	 */
	wxString get_rp_name();
	
	/**
	 * Get the names of the attributes that the relying party is asking for
	 * @return the names of the attributes that the relying party is asking for
	 */
	std::vector<wxString> get_rp_attributes();
	
	/**
	 * Set whether or not the "always" button should be shown in the consent dialog
	 * @param show_always if set to true, the always button will be shown in the consent dialog
	 */
	void set_show_always(bool show_always);
	
	/**
	 * Get whether or not the "always" button should be shown in the consent dialog
	 * @return true if the "always" button should be shown
	 */
	bool get_show_always();
	
	/**
	 * Set the consent result
	 * @param consent_result the consent result
	 */
	void set_consent_result(int consent_result);
	
	/**
	 * Retrieve the PIN (in case of PVT_REQUESTPIN event)
	 * @return the PIN entered by the user
	 */
	std::string get_pin();
	
	/**
	 * Retrieve the user consent result (in case of PEVT_REQUESTCONSENT event)
	 * @return the consent result
	 */
	int get_consent_result();
	
	/**
	 * Get event type
	 * @return the event type
	 */
	int get_type();
	
	/**
	 * Signal that the event has been handled
	 */
	void signal_handled();
	
	/**
	 * Clone the event
	 */
	wxEvent* Clone() const;
	
	DECLARE_DYNAMIC_CLASS( pivacy_ui_event );

private:
	// Event type
	int type;

	// Event input parameters
	int show_status;
	bool show_always;
	wxString rp_name;
	std::vector<wxString> rp_attributes;
	wxString msg;
	
	// Event return data
	pivacy_ui_event_data* evt_data;
	
	// Condition used to wait for handling of the event
	wxMutex* wait_mutex;
	wxCondition* wait_cond;
};

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE( pvEVT_PIVACYEVENT, 1 )
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*pivacy_event_fn)(pivacy_ui_event&);

#define EVT_PIVACYEVENT(func) \
	DECLARE_EVENT_TABLE_ENTRY( pvEVT_PIVACYEVENT, 			\
		-1, 												\
		-1,													\
		(wxObjectEventFunction)	(pivacy_event_fn)& func,	\
		(wxObject*) NULL),

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
	 * Pivacy UI event handler
	 * @param event the Pivacy UI event
	 */
	void on_pivacy_ui_evt(pivacy_ui_event& event);
	
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
	
	// Communications thread
	pivacy_ui_comm_thread* comm_thread;
	
	// Subdialogs
	pivacy_ui_pin_dialog* pin_dialog;
	pivacy_ui_consent_dialog* consent_dialog;
	pivacy_ui_status_dialog* status_dialog;
};

#endif // !_PIVACY_UI_CANVAS_H
