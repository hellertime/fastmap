check_PROGRAMS += \
	t/fastmap_attr_t \
	t/fastmap_outhandle_t \
	t/fastmap_inhandle_t \
	t/fastmap_pair_t \
	t/fastmap_block_t \
	t/fastmap_block_inline_t \
	t/fastmap_blob_t \
	t/1M_atom_t \
	t/1M_pair_t \
	t/1M_block_t \
	t/1M_block_inline_t \
	t/1M_blob_t \
	t/variable_block_test

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

t_1M_atom_t_SOURCES = t/1M_atom_t.c
t_1M_atom_t_LDADD = libtap.a src/libfastmap.la

t_1M_pair_t_SOURCES = t/1M_pair_t.c
t_1M_pair_t_LDADD = libtap.a src/libfastmap.la

t_1M_block_t_SOURCES = t/1M_block_t.c
t_1M_block_t_LDADD = libtap.a src/libfastmap.la

t_1M_block_inline_t_SOURCES = t/1M_block_inline_t.c
t_1M_block_inline_t_LDADD = libtap.a src/libfastmap.la

t_1M_blob_t_SOURCES = t/1M_blob_t.c
t_1M_blob_t_LDADD = libtap.a src/libfastmap.la

t_variable_block_test_SOURCES = t/variable_block_test.c
t_variable_block_test_LDADD = libtap.a src/libfastmap.la
