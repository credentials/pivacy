# $Id$

MAINTAINERCLEANFILES = 			$(srcdir)/Makefile.in

AM_CPPFLAGS = 				-I$(srcdir)/.. \
					-I$(srcdir)/../.. \
					-I$(srcdir)/../lib \
					-I$(srcdir)/../common \
					-I$(srcdir)/../../include 

noinst_PROGRAMS =			pivacy_ui_client_sample

pivacy_ui_client_sample_SOURCES =	pivacy_ui_client_sample.cpp

pivacy_ui_client_sample_LDADD =		../lib/libpivacy_ui.la
