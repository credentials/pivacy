# $Id$

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				-I$(srcdir)/../../include 

lib_LTLIBRARIES =		libpivacy_ui.la

libpivacy_ui_la_SOURCES =	pivacy_ui_lib_export.cpp \
				../common/pivacy_ui_proto.h

libpivacy_ui_la_LDFLAGS =	-version-info @PIVACY_UI_VERSION_INFO@

EXTRA_DIST =			$(srcdir)/../../include/*.h
