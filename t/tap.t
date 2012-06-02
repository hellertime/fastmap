#!/usr/bin/env perl
use strict;
use warnings;
use Test::More tests => 1;
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
