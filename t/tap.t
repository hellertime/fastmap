#!/usr/bin/env perl
use strict;
use warnings;
use Test::More tests => 13;
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

eq_or_diff ~~ `t/fastmap_pair_t 2>&1`, <<'END', "fastmap_pair_t";
1..16
ok 1 - fastmap_outhandle_put("0001")
ok 2 - fastmap_outhandle_put("0002")
ok 3 - fastmap_outhandle_put("0003")
ok 4 - fastmap_outhandle_put("0004")
ok 5 - fastmap_outhandle_put("0005")
ok 6 - fastmap_inhandle_get("0001")
ok 7 - aaaa == aaaa
ok 8 - fastmap_inhandle_get("0002")
ok 9 - bbbb == bbbb
ok 10 - fastmap_inhandle_get("0003")
ok 11 - cccc == cccc
ok 12 - fastmap_inhandle_get("0004")
ok 13 - dddd == dddd
ok 14 - fastmap_inhandle_get("0005")
ok 15 - ffff == ffff
ok 16 - unlink()
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

eq_or_diff ~~ `t/variable_block_test 2>&1`, <<'END', "variable_block_test"
1..144
ok 1 - Starting Test (4, 4)
ok 2 - write phase complete
ok 3 - read phase starting
ok 4 - Finished Test (4, 4)
ok 5 - Starting Test (4, 5)
ok 6 - write phase complete
ok 7 - read phase starting
ok 8 - Finished Test (4, 5)
ok 9 - Starting Test (4, 6)
ok 10 - write phase complete
ok 11 - read phase starting
ok 12 - Finished Test (4, 6)
ok 13 - Starting Test (4, 7)
ok 14 - write phase complete
ok 15 - read phase starting
ok 16 - Finished Test (4, 7)
ok 17 - Starting Test (4, 8)
ok 18 - write phase complete
ok 19 - read phase starting
ok 20 - Finished Test (4, 8)
ok 21 - Starting Test (4, 9)
ok 22 - write phase complete
ok 23 - read phase starting
ok 24 - Finished Test (4, 9)
ok 25 - Starting Test (5, 4)
ok 26 - write phase complete
ok 27 - read phase starting
ok 28 - Finished Test (5, 4)
ok 29 - Starting Test (5, 5)
ok 30 - write phase complete
ok 31 - read phase starting
ok 32 - Finished Test (5, 5)
ok 33 - Starting Test (5, 6)
ok 34 - write phase complete
ok 35 - read phase starting
ok 36 - Finished Test (5, 6)
ok 37 - Starting Test (5, 7)
ok 38 - write phase complete
ok 39 - read phase starting
ok 40 - Finished Test (5, 7)
ok 41 - Starting Test (5, 8)
ok 42 - write phase complete
ok 43 - read phase starting
ok 44 - Finished Test (5, 8)
ok 45 - Starting Test (5, 9)
ok 46 - write phase complete
ok 47 - read phase starting
ok 48 - Finished Test (5, 9)
ok 49 - Starting Test (6, 4)
ok 50 - write phase complete
ok 51 - read phase starting
ok 52 - Finished Test (6, 4)
ok 53 - Starting Test (6, 5)
ok 54 - write phase complete
ok 55 - read phase starting
ok 56 - Finished Test (6, 5)
ok 57 - Starting Test (6, 6)
ok 58 - write phase complete
ok 59 - read phase starting
ok 60 - Finished Test (6, 6)
ok 61 - Starting Test (6, 7)
ok 62 - write phase complete
ok 63 - read phase starting
ok 64 - Finished Test (6, 7)
ok 65 - Starting Test (6, 8)
ok 66 - write phase complete
ok 67 - read phase starting
ok 68 - Finished Test (6, 8)
ok 69 - Starting Test (6, 9)
ok 70 - write phase complete
ok 71 - read phase starting
ok 72 - Finished Test (6, 9)
ok 73 - Starting Test (7, 4)
ok 74 - write phase complete
ok 75 - read phase starting
ok 76 - Finished Test (7, 4)
ok 77 - Starting Test (7, 5)
ok 78 - write phase complete
ok 79 - read phase starting
ok 80 - Finished Test (7, 5)
ok 81 - Starting Test (7, 6)
ok 82 - write phase complete
ok 83 - read phase starting
ok 84 - Finished Test (7, 6)
ok 85 - Starting Test (7, 7)
ok 86 - write phase complete
ok 87 - read phase starting
ok 88 - Finished Test (7, 7)
ok 89 - Starting Test (7, 8)
ok 90 - write phase complete
ok 91 - read phase starting
ok 92 - Finished Test (7, 8)
ok 93 - Starting Test (7, 9)
ok 94 - write phase complete
ok 95 - read phase starting
ok 96 - Finished Test (7, 9)
ok 97 - Starting Test (8, 4)
ok 98 - write phase complete
ok 99 - read phase starting
ok 100 - Finished Test (8, 4)
ok 101 - Starting Test (8, 5)
ok 102 - write phase complete
ok 103 - read phase starting
ok 104 - Finished Test (8, 5)
ok 105 - Starting Test (8, 6)
ok 106 - write phase complete
ok 107 - read phase starting
ok 108 - Finished Test (8, 6)
ok 109 - Starting Test (8, 7)
ok 110 - write phase complete
ok 111 - read phase starting
ok 112 - Finished Test (8, 7)
ok 113 - Starting Test (8, 8)
ok 114 - write phase complete
ok 115 - read phase starting
ok 116 - Finished Test (8, 8)
ok 117 - Starting Test (8, 9)
ok 118 - write phase complete
ok 119 - read phase starting
ok 120 - Finished Test (8, 9)
ok 121 - Starting Test (9, 4)
ok 122 - write phase complete
ok 123 - read phase starting
ok 124 - Finished Test (9, 4)
ok 125 - Starting Test (9, 5)
ok 126 - write phase complete
ok 127 - read phase starting
ok 128 - Finished Test (9, 5)
ok 129 - Starting Test (9, 6)
ok 130 - write phase complete
ok 131 - read phase starting
ok 132 - Finished Test (9, 6)
ok 133 - Starting Test (9, 7)
ok 134 - write phase complete
ok 135 - read phase starting
ok 136 - Finished Test (9, 7)
ok 137 - Starting Test (9, 8)
ok 138 - write phase complete
ok 139 - read phase starting
ok 140 - Finished Test (9, 8)
ok 141 - Starting Test (9, 9)
ok 142 - write phase complete
ok 143 - read phase starting
ok 144 - Finished Test (9, 9)
END
