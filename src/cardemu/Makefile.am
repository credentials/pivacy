# $Id$

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				-I$(srcdir)/../../include \
				@XML_CFLAGS@ \
				@SILVIA_CFLAGS@ \
				@EDNA_CFLAGS@ \
				@LIBCONFIG_CFLAGS@

bin_PROGRAMS =			pivacy_cardemu

pivacy_cardemu_SOURCES =	pivacy_cardemu.cpp \
				pivacy_cardemu_emulator.cpp \
				pivacy_cardemu_emulator.h \
				../common/pivacy_config.cpp \
				../common/pivacy_config.h \
				../common/pivacy_log.cpp \
				../common/pivacy_log.h \
				../common/pivacy_errors.h \
				../common/pivacy_cred_xml_rw.cpp \
				../common/pivacy_cred_xml_rw.h \
				../common/pivacy_credential.cpp \
				../common/pivacy_credential.h \
				../../include/pivacy_ui_lib.h

pivacy_cardemu_LDADD =		@XML_LIBS@ \
				@SILVIA_LIBS@ \
				@EDNA_LIBS@ \
				@LIBCONFIG_LIBS@ \
				../lib/libpivacy_ui.la
