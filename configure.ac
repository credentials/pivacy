dnl
dnl $Id$
dnl

################################################################################
# Version information                                                          #
################################################################################

# Program version

define([PIVACY_VERSION_MAJOR], [0])
define([PIVACY_VERSION_MINOR], [1])
define([PIVACY_VERSION_BUILDREV], [0])
define([PACKAGE_SUFFIX], [])

# Library version

# Code changed:                      PIVACY_UI_VERSION_REVISION++
# Interface added/removed/changed:   PIVACY_UI_VERSION_CURRENT++, PIVACY_UI_VERSION_REVISION=0
# Interface added:                   PIVACY_UI_VERSION_AGE++
# Interface removed:                 PIVACY_UI_VERSION_AGE=0

define([PIVACY_UI_VERSION_CURRENT], [0])
define([PIVACY_UI_VERSION_AGE], [0])
define([PIVACY_UI_VERSION_REVISION], [0])

################################################################################
# Configuration functions                                                      #
################################################################################

# Init
AC_PREREQ(2.61)
AC_INIT([pivacy],[PIVACY_VERSION_MAJOR.PIVACY_VERSION_MINOR.PIVACY_VERSION_BUILDREV[]PACKAGE_SUFFIX])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([src/Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE(foreign)
ACX_PREFIXHACK

# Checks for compilers and other programs
AC_PROG_CC_C99
AC_PROG_CXX

AC_PROG_INSTALL

# Compiler flags
ACX_PEDANTIC
ACX_STRICT

# Libtool settings
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# Version info for the library
PIVACY_UI_VERSION_INFO="PIVACY_UI_VERSION_CURRENT:PIVACY_UI_VERSION_REVISION:PIVACY_UI_VERSION_AGE"
AC_SUBST(PIVACY_UI_VERSION_INFO)

# Log level
AC_ARG_WITH(
	[loglevel],
	[AS_HELP_STRING([--with-loglevel=INT],[The log level. 0=No log 1=Error 2=Warning 3=Info 4=Debug (default INT=3)])],
	[PIVACY_LOGLEVEL="$withval"],
	[PIVACY_LOGLEVEL=3]
)

# Screen width
AC_ARG_WITH(
	[screenwidth],
	[AS_HELP_STRING([--with-screenwidth=INT],[The screen width (default INT=320)])],
	[PIVACY_SCREENWIDTH="$withval"],
	[PIVACY_SCREENWIDTH=320])

# Screen height
AC_ARG_WITH(
	[screenheight],
	[AS_HELP_STRING([--with-screenheight=INT],[The screen height (default INT=240)])],
	[PIVACY_SCREENHEIGHT="$withval"],
	[PIVACY_SCREENHEIGHT=240])

AM_OPTIONS_WXCONFIG

reqwx=2.4.0

AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in your PATH, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in the LD_LIBRARY_PATH or an equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

PKG_CHECK_MODULES([LIBCONFIG], [libconfig >= 1.3.2],, AC_MSG_ERROR([libconfig 1.3.2 or newer not found]))

PKG_CHECK_MODULES([EDNA], [edna >= 0.1.0],, AC_MSG_ERROR([edna 0.1.0 or newer not found]))

PKG_CHECK_MODULES([SILVIA], [silvia >= 0.1.1],, AC_MSG_ERROR([silvia 0.1.1 or newer not found]))

PKG_CHECK_MODULES([XML], [libxml-2.0 >= 2.0], , AC_MSG_ERROR([libxml2 2.0 or newer not found]))

# Check for headers
AC_HEADER_STDC

# Check for functions
AC_FUNC_MEMCMP

# Define default paths
full_sysconfdir=`eval eval eval eval eval echo "${sysconfdir}" | sed "s#NONE#${prefix}#" | sed "s#NONE#${ac_default_prefix}#"`
full_localstatedir=`eval eval eval eval eval echo "${localstatedir}" | sed "s#NONE#${prefix}#" | sed "s#NONE#${ac_default_prefix}#"`
full_libdir=`eval eval eval eval eval echo "${libdir}" | sed "s#NONE#${prefix}#" | sed "s#NONE#${ac_default_prefix}#"`
default_pivacy_cardemu_conf="`eval echo ${full_sysconfdir} | sed s,NONE,$ac_default_prefix,g`/pivacy_cardemu.conf"
default_pivacy_cardemu_pid_file="`eval echo ${full_localstatedir} | sed s,NONE,$ac_default_prefix,g`/run/pivacy_cardemu.pid"
default_pivacy_ui_conf="`eval echo ${full_sysconfdir} | sed s,NONE,$ac_default_prefix,g`/pivacy_ui.conf"
default_pivacy_ui_pid_file="`eval echo ${full_localstatedir} | sed s,NONE,$ac_default_prefix,g`/run/pivacy_ui.pid"

# Define some variables for the code
AC_DEFINE_UNQUOTED(
	[VERSION_MAJOR],
	[PIVACY_VERSION_MAJOR],
	[Major version number]
)
AC_DEFINE_UNQUOTED(
	[VERSION_MINOR],
	[PIVACY_VERSION_MINOR],
	[Minor version number]
)
AC_DEFINE_UNQUOTED(
	[PIVACY_LOGLEVEL],
	[$PIVACY_LOGLEVEL],
	[The log level set by the user]
)
AC_DEFINE_UNQUOTED(
	[PIVACY_SCREENWIDTH],
	[$PIVACY_SCREENWIDTH],
	[The screen width]
)
AC_DEFINE_UNQUOTED(
	[PIVACY_SCREENHEIGHT],
	[$PIVACY_SCREENHEIGHT],
	[The screen height]
)
AC_DEFINE_UNQUOTED(
	[DEFAULT_PIVACY_CARDEMU_CONF],
	["$default_pivacy_cardemu_conf"],
	[The default configuration file for the card emulator]
)
AC_DEFINE_UNQUOTED(
	[DEFAULT_PIVACY_CARDEMU_PIDFILE],
	["$default_pivacy_cardemu_pid_file"],
	[The default PID file for the card emulator]
)
AC_DEFINE_UNQUOTED(
	[DEFAULT_PIVACY_UI_CONF],
	["$default_pivacy_cardemu_conf"],
	[The default configuration file for the UI]
)
AC_DEFINE_UNQUOTED(
	[DEFAULT_PIVACY_UI_PIDFILE],
	["$default_pivacy_cardemu_pid_file"],
	[The default PID file for the card UI]
)

# Generate theinstall script
AC_PROG_INSTALL

# Generate the makefiles
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/lib/Makefile
	src/ui/Makefile
	src/credgen/Makefile
	src/cardemu/Makefile
	src/samples/Makefile
])

AC_OUTPUT
