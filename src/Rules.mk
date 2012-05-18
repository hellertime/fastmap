lib_LTLIBRARIES += src/libfastmap.la

src_libfastmap_la_SOURCES = \
	fastmap_attr.c \
	fastmap_datumattr.c \
	fastmap_datum.c \
	fastmap.c

src_libfastmap_la_LDFLAGS = -avoid-version
