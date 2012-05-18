##
# libfastmap Autoconf script
#
# Author: Chris Heller <hellertime@gmail.com>
#

AC_INIT([Fastmap Library],[0.0.1],[hellertime@gmail.com],[libfastmap])

# Configure Autoconf
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([fastmap_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([include/fastmap.h])

# Check for programs
AC_PROG_CC

AC_PROG_LIBTOOL

# Check for system features
AC_C_BIGENDIAN
AC_SYS_LARGEFILE

# Check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Lots of automake warnings
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

# *Required when using subdir-objects* 
AM_PROG_CC_C_O

AC_OUTPUT([Makefile])
