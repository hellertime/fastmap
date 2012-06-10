#!/usr/bin/env perl
use strict;
use warnings;
use Test::More tests => 11;
use Test::Differences;

eq_or_diff ~~ `t/fastmap_attr_t 2>&1`, <<'END', "fastmap_attr_t";
1..12
ok 1 - fastmap_attr_init(NULL)
ok 2 - fastmap_attr_init()
ok 3 - fastmap_attr_setrecords()
ok 4 - fastmap_attr_getrecords()
ok 5 - fastmap_attr_setksize()
ok 6 - fastmap_attr_getksize()
ok 7 - fastmap_attr_setvsize()
ok 8 - fastmap_attr_getvsize()
ok 9 - fastmap_attr_setformat()
ok 10 - fastmap_attr_getformat()
ok 11 - fastmap_attr_destroy(NULL)
ok 12 - fastmap_attr_destroy()
END

eq_or_diff ~~ `t/fastmap_outhandle_t 2>&1`, <<'END', "fastmap_outhandle_t";
1..12
ok 1 - fastmap_outhandle_init(NULL_HANDLE)
ok 2 - fastmap_outhandle_init(NULL_ATTR)
ok 3 - fastmap_outhandle_init("")
ok 4 - fastmap_outhandle_init()
ok 5 - fastmap_outhandle_put(0001)
ok 6 - fastmap_outhandle_put(0002)
ok 7 - fastmap_outhandle_put(0003)
ok 8 - fastmap_outhandle_put(0004)
ok 9 - fastmap_outhandle_put(0005)
ok 10 - fastmap_outhandle_destroy(NULL)
ok 11 - fastmap_outhandle_destroy()
ok 12 - unlink()
END

eq_or_diff ~~ `t/fastmap_inhandle_t 2>&1`, <<'END', "fastmap_inhandle_t";
1..21
ok 1 - fastmap_outhandle_put() == TOO_MANY_RECORDS
ok 2 - fastmap_inhandle_init(NULL)
ok 3 - fastmap_inhandle_init("")
ok 4 - fastmap_inhandle_init()
ok 5 - fastmap_inhandle_setcmpfunc()
ok 6 - fastmap_inhandle_getattr()
ok 7 - fastmap_attr_getrecords()
ok 8 - records: 5
ok 9 - fastmap_attr_getksize()
ok 10 - key size: 4
ok 11 - fastmap_attr_getformat()
ok 12 - format: FASTMAP_ATOM
ok 13 - fastmap_inhandle_get("0001")
ok 14 - fastmap_inhandle_get("0002")
ok 15 - fastmap_inhandle_get("0003")
ok 16 - fastmap_inhandle_get("0004")
ok 17 - fastmap_inhandle_get("0005")
ok 18 - fastmap_inhandle_get("0006") == NOT_FOUND
ok 19 - fastmap_inhandle_destroy(NULL)
ok 20 - fastmap_inhandle_destroy()
ok 21 - unlink()
END

eq_or_diff ~~ `t/fastmap_block_t 2>&1`, <<'END', "fastmap_block_t";
1..18
ok 1 - !INLINE_BLOCK
ok 2 - fastmap_outhandle_put("0001")
ok 3 - fastmap_outhandle_put("0002")
ok 4 - fastmap_outhandle_put("0003")
ok 5 - fastmap_outhandle_put("0004")
ok 6 - fastmap_outhandle_put("0005")
ok 7 - !INLINE_BLOCKS
ok 8 - fastmap_inhandle_get("0001")
ok 9 - aaaax == aaaax
ok 10 - fastmap_inhandle_get("0002")
ok 11 - bbbbx == bbbbx
ok 12 - fastmap_inhandle_get("0003")
ok 13 - ccccx == ccccx
ok 14 - fastmap_inhandle_get("0004")
ok 15 - ddddx == ddddx
ok 16 - fastmap_inhandle_get("0005")
ok 17 - ffffx == ffffx
ok 18 - unlink()
END

eq_or_diff ~~ `t/fastmap_block_inline_t 2>&1`, <<'END', "fastmap_block_inline_t";
1..18
ok 1 - INLINE_BLOCK
ok 2 - fastmap_outhandle_put("0001")
ok 3 - fastmap_outhandle_put("0002")
ok 4 - fastmap_outhandle_put("0003")
ok 5 - fastmap_outhandle_put("0004")
ok 6 - fastmap_outhandle_put("0005")
ok 7 - INLINE_BLOCKS
ok 8 - fastmap_inhandle_get("0001")
ok 9 - aaaaxyzz == aaaaxyzz
ok 10 - fastmap_inhandle_get("0002")
ok 11 - bbbbxyxx == bbbbxyxx
ok 12 - fastmap_inhandle_get("0003")
ok 13 - ccccxyab == ccccxyab
ok 14 - fastmap_inhandle_get("0004")
ok 15 - ddddxycc == ddddxycc
ok 16 - fastmap_inhandle_get("0005")
ok 17 - ffffxdef == ffffxdef
ok 18 - unlink()
END

eq_or_diff ~~ `t/fastmap_blob_t 2>&1`, <<'END', "fastmap_blob_t";
1..16
ok 1 - fastmap_outhandle_put("0001")
ok 2 - fastmap_outhandle_put("0002")
ok 3 - fastmap_outhandle_put("0003")
ok 4 - fastmap_outhandle_put("0004")
ok 5 - fastmap_outhandle_put("0005")
ok 6 - fastmap_inhandle_get("0001")
ok 7 - foo == foo
ok 8 - fastmap_inhandle_get("0002")
ok 9 - bar == bar
ok 10 - fastmap_inhandle_get("0003")
ok 11 - baz == baz
ok 12 - fastmap_inhandle_get("0004")
ok 13 - quux == quux
ok 14 - fastmap_inhandle_get("0005")
ok 15 - quorum == quorum
ok 16 - unlink()
END

eq_or_diff ~~ `t/1M_atom_t 2>&1`, <<'END', "1M_atom_t";
1..26
ok 1 - created fastmap
ok 2 - success after: 0000000 writes
ok 3 - success after: 0100000 writes
ok 4 - success after: 0200000 writes
ok 5 - success after: 0300000 writes
ok 6 - success after: 0400000 writes
ok 7 - success after: 0500000 writes
ok 8 - success after: 0600000 writes
ok 9 - success after: 0700000 writes
ok 10 - success after: 0800000 writes
ok 11 - success after: 0900000 writes
ok 12 - success after: 1000000 writes
ok 13 - saved fastmap
ok 14 - opened fastmap
ok 15 - success after: 0000000 reads
ok 16 - success after: 0100000 reads
ok 17 - success after: 0200000 reads
ok 18 - success after: 0300000 reads
ok 19 - success after: 0400000 reads
ok 20 - success after: 0500000 reads
ok 21 - success after: 0600000 reads
ok 22 - success after: 0700000 reads
ok 23 - success after: 0800000 reads
ok 24 - success after: 0900000 reads
ok 25 - success after: 1000000 reads
ok 26 - closed fastmap
END

eq_or_diff ~~ `t/1M_pair_t 2>&1`, <<'END', "1M_pair_t";
1..26
ok 1 - created fastmap
ok 2 - success after: 0000000 writes
ok 3 - success after: 0100000 writes
ok 4 - success after: 0200000 writes
ok 5 - success after: 0300000 writes
ok 6 - success after: 0400000 writes
ok 7 - success after: 0500000 writes
ok 8 - success after: 0600000 writes
ok 9 - success after: 0700000 writes
ok 10 - success after: 0800000 writes
ok 11 - success after: 0900000 writes
ok 12 - success after: 1000000 writes
ok 13 - saved fastmap
ok 14 - opened fastmap
ok 15 - success after: 0000000 reads
ok 16 - success after: 0100000 reads
ok 17 - success after: 0200000 reads
ok 18 - success after: 0300000 reads
ok 19 - success after: 0400000 reads
ok 20 - success after: 0500000 reads
ok 21 - success after: 0600000 reads
ok 22 - success after: 0700000 reads
ok 23 - success after: 0800000 reads
ok 24 - success after: 0900000 reads
ok 25 - success after: 1000000 reads
ok 26 - closed fastmap
END

eq_or_diff ~~ `t/1M_block_t 2>&1`, <<'END', "1M_block_t";
1..26
ok 1 - created fastmap
ok 2 - success after: 0000000 writes
ok 3 - success after: 0100000 writes
ok 4 - success after: 0200000 writes
ok 5 - success after: 0300000 writes
ok 6 - success after: 0400000 writes
ok 7 - success after: 0500000 writes
ok 8 - success after: 0600000 writes
ok 9 - success after: 0700000 writes
ok 10 - success after: 0800000 writes
ok 11 - success after: 0900000 writes
ok 12 - success after: 1000000 writes
ok 13 - saved fastmap
ok 14 - opened fastmap
ok 15 - success after: 0000000 reads
ok 16 - success after: 0100000 reads
ok 17 - success after: 0200000 reads
ok 18 - success after: 0300000 reads
ok 19 - success after: 0400000 reads
ok 20 - success after: 0500000 reads
ok 21 - success after: 0600000 reads
ok 22 - success after: 0700000 reads
ok 23 - success after: 0800000 reads
ok 24 - success after: 0900000 reads
ok 25 - success after: 1000000 reads
ok 26 - closed fastmap
END

eq_or_diff ~~ `t/1M_block_inline_t 2>&1`, <<'END', "1M_block_inline_t";
1..26
ok 1 - created fastmap
ok 2 - success after: 0000000 writes
ok 3 - success after: 0100000 writes
ok 4 - success after: 0200000 writes
ok 5 - success after: 0300000 writes
ok 6 - success after: 0400000 writes
ok 7 - success after: 0500000 writes
ok 8 - success after: 0600000 writes
ok 9 - success after: 0700000 writes
ok 10 - success after: 0800000 writes
ok 11 - success after: 0900000 writes
ok 12 - success after: 1000000 writes
ok 13 - saved fastmap
ok 14 - opened fastmap
ok 15 - success after: 0000000 reads
ok 16 - success after: 0100000 reads
ok 17 - success after: 0200000 reads
ok 18 - success after: 0300000 reads
ok 19 - success after: 0400000 reads
ok 20 - success after: 0500000 reads
ok 21 - success after: 0600000 reads
ok 22 - success after: 0700000 reads
ok 23 - success after: 0800000 reads
ok 24 - success after: 0900000 reads
ok 25 - success after: 1000000 reads
ok 26 - closed fastmap
END

eq_or_diff ~~ `t/1M_blob_t 2>&1`, <<'END', "1M_blob_t";
1..26
ok 1 - created fastmap
ok 2 - success after: 0000000 writes
ok 3 - success after: 0100000 writes
ok 4 - success after: 0200000 writes
ok 5 - success after: 0300000 writes
ok 6 - success after: 0400000 writes
ok 7 - success after: 0500000 writes
ok 8 - success after: 0600000 writes
ok 9 - success after: 0700000 writes
ok 10 - success after: 0800000 writes
ok 11 - success after: 0900000 writes
ok 12 - success after: 1000000 writes
ok 13 - saved fastmap
ok 14 - opened fastmap
ok 15 - success after: 0000000 reads
ok 16 - success after: 0100000 reads
ok 17 - success after: 0200000 reads
ok 18 - success after: 0300000 reads
ok 19 - success after: 0400000 reads
ok 20 - success after: 0500000 reads
ok 21 - success after: 0600000 reads
ok 22 - success after: 0700000 reads
ok 23 - success after: 0800000 reads
ok 24 - success after: 0900000 reads
ok 25 - success after: 1000000 reads
ok 26 - closed fastmap
END
