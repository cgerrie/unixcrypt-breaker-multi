## Copyright (C) 2006-2013 Peter Selinger.
## This file is part of unixcrypt-breaker. It is free software and it is
## distributed under the terms of the GNU general public license. See
## the file COPYING for details.

## Process this file with automake to produce Makefile.in

bin_PROGRAMS = unixcrypt-breaker
EXTRA_DIST = getopt.c getopt1.c getopt.h

unixcrypt_breaker_SOURCES = unixcrypt-breaker.c stats.c stats.h block.c block.h

unixcrypt_breaker_LDADD = @EXTRA_OBJS@
unixcrypt_breaker_DEPENDENCIES = @EXTRA_OBJS@
