lib_LTLIBRARIES += src/libfastmap.la

src_libfastmap_la_SOURCES = \
	src/fastmap_attr.c \
	src/fastmap_datumattr.c \
	src/fastmap_datum.c \
	src/fastmap.c

src_libfastmap_la_LDFLAGS = -avoid-version
