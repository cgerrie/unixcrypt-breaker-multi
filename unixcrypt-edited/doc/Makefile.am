## Copyright (C) 2006-2013 Peter Selinger.
## This file is part of unixcrypt-breaker. It is free software and it is
## distributed under the terms of the GNU general public license. See
## the file COPYING for details.

## Process this file with automake to produce Makefile.in

EXTRA_DIST = unixcrypt-breaker.1.in man2html.sed

DISTCLEANFILES = unixcrypt-breaker.ps unixcrypt-breaker.html

man_MANS = unixcrypt-breaker.1

ps: unixcrypt-breaker.ps
html: unixcrypt-breaker.html

unixcrypt-breaker.ps: unixcrypt-breaker.1
	groff -man unixcrypt-breaker.1 > unixcrypt-breaker.ps

unixcrypt-breaker.html: unixcrypt-breaker.1 $(srcdir)/man2html.sed
	cat unixcrypt-breaker.1 | sed -f $(srcdir)/man2html.sed > unixcrypt-breaker.html
