##
# Top-level Makefile for libfastmap
#

# headers location
AM_CPPFLAGS = -I$(top_srcdir)/include

# m4 path
ACLOCAL_AMFLAGS = -I m4

# Makefile fragments will complete these variables
bin_PROGRAMS =
lib_LTLIBRARIES =

TEST =
check_PROGRAMS =

nobase_include_HEADERS =
include_HEADERS = include/fasttype.h

# makefile fragments
include src/Rules.mk

really-clean: maintainer-clean
	find $(top_srcdir) -name \*.in -exec rm {} \;
	find $(top_srcdir) -name \*.in~ -exec rm {} \;
	rm $(top_srcdir)/m4/*
	rm $(top_srcdir)/build-aux/*
	rmdir $(top_srcdir)/m4
	rmdir $(top_srcdir)/build-aux
	rm $(top_srcdir)/aclocal.m4
	rm $(top_srcdir)/configure

.PHONY: really-clean
