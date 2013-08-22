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
 pivacy_ui_status.h

 The Pivacy UI consent dialog
 *****************************************************************************/
 
#ifndef _PIVACY_UI_STATUS_H
#define _PIVACY_UI_STATUS_H
 
#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h" 
#endif // WX_PRECOMP

#include "pivacy_ui_canvas.h"
#include <string>
#include <list>

class pivacy_ui_status_dialog : public pivacy_ui_ux_base
{
public:
	/**
	 * Constructor
	 * @param status the status to display
	 */
	pivacy_ui_status_dialog(int status);
	
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
	
	enum
	{
		PIVACY_STATUS_OK,
		PIVACY_STATUS_WARN,
		PIVACY_STATUS_FAIL,
		PIVACY_STATUS_WAIT,
		PIVACY_STATUS_PRESENT
	};
	
private:
	wxImage status_image;
};

#endif // !_PIVACY_UI_STATUS_H
