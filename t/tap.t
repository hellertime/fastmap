#!/usr/bin/env perl
use strict;
use warnings;
use Test::More tests => 3;
use Test::Differences;

eq_or_diff ~~ `t/fastmap_attr_t 2>&1`, <<'END', "fastmap_attr_t";
1..12
ok 1 - fastmap_attr_init(NULL)
ok 2 - fastmap_attr_init()
ok 3 - fastmap_attr_setelements()
ok 4 - fastmap_attr_getelements()
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
1..20
ok 1 - fastmap_inhandle_init(NULL)
ok 2 - fastmap_inhandle_init("")
ok 3 - fastmap_inhandle_init()
ok 4 - fastmap_inhandle_setcmpfunc()
ok 5 - fastmap_inhandle_getattr()
ok 6 - fastmap_attr_getelements()
ok 7 - elements: 5
ok 8 - fastmap_attr_getksize()
ok 9 - key size: 4
ok 10 - fastmap_attr_getformat()
ok 11 - format: FASTMAP_ATOM
ok 12 - fastmap_inhandle_get("0001")
ok 13 - fastmap_inhandle_get("0002")
ok 14 - fastmap_inhandle_get("0003")
ok 15 - fastmap_inhandle_get("0004")
ok 16 - fastmap_inhandle_get("0005")
ok 17 - fastmap_inhandle_get("0006")
ok 18 - fastmap_inhandle_destroy(NULL)
ok 19 - fastmap_inhandle_destroy()
ok 20 - unlink()
END
