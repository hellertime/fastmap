#!/usr/bin/env perl
use strict;
use warnings;
use Test::More tests => 1;
use Test::Differences;

eq_or_diff ~~ `t/simple 2>&1`, <<'END', "simple";
1..1
ok 1
END
