dnl  Copyright (C) 2006-2013 Peter Selinger.
dnl  This file is part of unixcrypt-breaker. It is free software and it is
dnl  distributed under the terms of the GNU general public license. See
dnl  the file COPYING for details.

dnl  Process this file with autoconf to produce a configure script.

dnl ----------------------------------------------------------------------
AC_INIT(unixcrypt-breaker, 0.2)
AC_CONFIG_SRCDIR(src/unixcrypt-breaker.c)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

DATE="June 2013"

dnl ----------------------------------------------------------------------
dnl Check for programs.
AC_PROG_CC

dnl ----------------------------------------------------------------------
dnl Checks for library functions.
AC_CHECK_FUNC(getopt_long, , EXTRA_OBJS="$EXTRA_OBJS getopt.o getopt1.o")

dnl ----------------------------------------------------------------------
dnl Set up substitutions of non-standard configuration parameters
AC_SUBST(DATE)
AC_SUBST(EXTRA_OBJS)

dnl ----------------------------------------------------------------------
AC_OUTPUT([doc/unixcrypt-breaker.1
	   Makefile 
	   src/Makefile 
	   doc/Makefile 
          ])
