check_PROGRAMS += \
	t/fastmap_attr_t \
	t/fastmap_outhandle_t \
	t/fastmap_inhandle_t \
	t/fastmap_pair_t \
	t/fastmap_block_t \
	t/fastmap_block_inline_t \
	t/fastmap_blob_t

t_fastmap_attr_t_SOURCES = t/fastmap_attr_t.c
t_fastmap_attr_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_outhandle_t_SOURCES = t/fastmap_outhandle_t.c
t_fastmap_outhandle_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_inhandle_t_SOURCES = t/fastmap_inhandle_t.c
t_fastmap_inhandle_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_pair_t_SOURCES = t/fastmap_pair_t.c
t_fastmap_pair_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_block_t_SOURCES = t/fastmap_block_t.c
t_fastmap_block_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_block_inline_t_SOURCES = t/fastmap_block_inline_t.c
t_fastmap_block_inline_t_LDADD = libtap.a src/libfastmap.la

t_fastmap_blob_t_SOURCES = t/fastmap_blob_t.c
t_fastmap_blob_t_LDADD = libtap.a src/libfastmap.la
