# $Id$

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				@WX_CPPFLAGS@

bin_PROGRAMS =			pivacy_ui

pivacy_ui_SOURCES =		pivacy_ui_app.cpp \
				pivacy_ui_app.h \
				pivacy_ui_canvas.cpp \
				pivacy_ui_canvas.h \
				pivacy_ui_colours.h

pivacy_ui_LDADD =		@WX_LIBS@
