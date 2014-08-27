#!/usr/bin/perl
# Copyright (C) 2011 by Alex Kozadaev <akozadaev at yahoo dot com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

use strict;
use warnings;

my $conf = {
    num     => {},
    port    => {},
    debug   => 0,
    version => "0.22",
  };

if (@ARGV) { 
  open PIPE, "<$ARGV[scalar @ARGV - 1]" or die "Cannot open file: $!\n";
} else {
  &usage;
  exit(0);
}

&readarg();
my $in = 0;

while (<PIPE>) {
  # reading new connection record
  if (/New \w+ connection #(\d+): [\d\.]+\((\d+)\)/) {
    print "DEBUG: $1 $2 $conf->{'num'} $conf->{'port'}\n" if $conf->{'debug'};

    if ($conf->{'num'}->{$1} || $conf->{'port'}->{$2}) {
      print;
      $conf->{'num'}->{$1}++;
      #$conf->{'port'}->{$2}++;  # fixing matching by the port even if searching based on connections.
      $in++;
    }
  } elsif (/^\s+/) {     # record belongs to the current record - printing
    print if $in;
  } elsif (/^(\d+)/) {   # checking if the record belongs to interesting connections
    if ($conf->{'num'}->{$1}) {
      print;
      $in++;
    } else {
      $in = 0;
    }
  }
}

#==[ subroutines ]==========================================

sub readarg {
  foreach (shift @ARGV) {
    if (/-h/) { &usage(); exit(0); }
    if (/-n/) {
      my $args = shift @ARGV;
      foreach my $arg (split /,/, $args) {
        $conf->{'num'}->{$arg}++;
      }
    }

    if (/-p/) {
      my $args = shift @ARGV;
      foreach my $arg (split /,/, $args) {
        $conf->{'port'}->{$arg}++;
      }
    }
  }
  if ((scalar keys %{$conf->{'num'}} < 1) && (scalar keys %{$conf->{'port'}} < 1)) {
    &usage;
    die "Either -n or -p must be specified\n";
  }
}

sub usage {
  print <<END
ssld-extract.pl v$conf->{version}

  ssld-extact.pl [-n x,y | -p n,m] [ssldump filename | - to read from pipe]

  Extract one or more connections from a SSL dump file.

  Usage:
    -n    comma separated list of connection numbers (no spaces allowed)
    -p    comma separated list of client port numbers (no spaces allowed)
    -h    this text

  PLEASE NOTE: -n overrides -p, so when used together -p will not have any effect.

END
}
