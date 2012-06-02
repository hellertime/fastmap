lib_LTLIBRARIES += src/libfastmap.la

src_libfastmap_la_SOURCES = \
	src/fastmap.c

# TODO: Add -ffast-math in production, optimizes floor/ceil (since we're only doing integer math)
src_libfastmap_la_CPPFLAGS = -I$(top_srcdir)/include -Wall -Wextra -Werror -pedantic -Wstrict-aliasing=2 -Wno-missing-field-initializers
src_libfastmap_la_LDFLAGS = -avoid-version -lm
