check_PROGRAMS += \
	t/fastmap_attr_t \
	t/fastmap_outhandle_t \
	t/fastmap_inhandle_t

t_fastmap_attr_t_SOURCES = t/fastmap_attr_t.c
t_fastmap_attr_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_outhandle_t_SOURCES = t/fastmap_outhandle_t.c
t_fastmap_outhandle_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_inhandle_t_SOURCES = t/fastmap_inhandle_t.c
t_fastmap_inhandle_t_LDADD = libtap.a src/libfastmap.la
