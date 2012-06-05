lib_LTLIBRARIES += src/libfastmap.la

src_libfastmap_la_SOURCES = \
	src/fastmap.c

# TODO: Add -ffast-math in production, optimizes floor/ceil (since we're only doing integer math)
src_libfastmap_la_CPPFLAGS = $(AM_CPPFLAGS) -Wall -Wextra -Werror -pedantic -Wstrict-aliasing=2 -Wno-missing-field-initializers
src_libfastmap_la_LDFLAGS = -avoid-version -lm

bin_PROGRAMS += \
	src/dumpfastmap \
	src/tofastmap

src_dumpfastmap_SOURCES = src/dumpfastmap.c
src_dumpfastmap_LDADD = src/libfastmap.la

src_tofastmap_SOURCES = src/tofastmap.c
src_tofastmap_LDADD = src/libfastmap.la
