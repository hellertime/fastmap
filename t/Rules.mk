check_PROGRAMS += t/fastmap_attr_t

t_fastmap_attr_t_SOURCES = t/fastmap_attr_t.c
t_fastmap_attr_t_LDADD = libtap.a src/libfastmap.la
