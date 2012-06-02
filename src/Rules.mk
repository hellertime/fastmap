lib_LTLIBRARIES += src/libfastmap.la

src_libfastmap_la_SOURCES = \
	src/fastmap.c

src_libfastmap_la_LDFLAGS = -avoid-version
