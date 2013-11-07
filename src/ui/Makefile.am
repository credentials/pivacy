# $Id$

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				-I$(srcdir)/../../include \
				@WX_CPPFLAGS@ \
				@LIBCONFIG_CFLAGS@

bin_PROGRAMS =			pivacy_ui

pivacy_ui_SOURCES =		pivacy_ui_app.cpp \
				pivacy_ui_app.h \
				pivacy_ui_canvas.cpp \
				pivacy_ui_canvas.h \
				pivacy_ui_colours.h \
				pivacy_ui_pindialog.cpp \
				pivacy_ui_pindialog.h \
				pivacy_ui_consent.cpp \
				pivacy_ui_consent.h \
				pivacy_ui_status.cpp \
				pivacy_ui_status.h \
				pivacy_ui_comm.cpp \
				pivacy_ui_comm.h \
				../common/pivacy_log.cpp \
				../common/pivacy_log.h \
				../common/pivacy_config.cpp \
				../common/pivacy_config.h \
				../../include/pivacy_ui_lib.h

pivacy_ui_LDADD =		@WX_LIBS@ \
				@LIBCONFIG_LIBS@
