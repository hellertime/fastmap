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
noinst_LIBRARIES = libtap.a
EXTRA_DIST = t/tap.t

check_PROGRAMS =

include_HEADERS = include/fastmap.h

libtap_a_SOURCES = tap.c tap.h
libtap_a_CPPFLAGS = -Wall

# makefile fragments
include src/Rules.mk
include t/Rules.mk

TESTS = $(check_PROGRAMS)

really-clean: maintainer-clean
	find $(top_srcdir) -name \*.in -exec rm {} \;
	find $(top_srcdir) -name \*.in~ -exec rm {} \;
	rm $(top_srcdir)/m4/*
	rm $(top_srcdir)/build-aux/*
	rmdir $(top_srcdir)/m4
	rmdir $(top_srcdir)/build-aux
	rm $(top_srcdir)/aclocal.m4
	rm $(top_srcdir)/configure
	rmdir $(top_builddir)/src
	rmdir $(top_builddir)/t

.PHONY: really-clean
