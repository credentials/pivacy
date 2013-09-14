# $Id$

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS = 			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				@XML_CFLAGS@ \
				@SILVIA_CFLAGS@

bin_PROGRAMS =			pivacy_credgen

pivacy_credgen_SOURCES =	pivacy_credgen.cpp \
				../common/pivacy_cred_xml_rw.cpp \
				../common/pivacy_cred_xml_rw.h \
				../common/pivacy_credential.cpp \
				../common/pivacy_credential.h

pivacy_credgen_LDADD =		@XML_LIBS@ \
				@SILVIA_LIBS@
