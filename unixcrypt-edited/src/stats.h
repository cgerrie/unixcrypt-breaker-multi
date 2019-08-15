/* Copyright (C) 2006-2013 Peter Selinger.  
   This file is part of unixcrypt-breaker. It is free software and it
   is distributed under the terms of the GNU general public license.
   There is no warranty. See the file COPYING for details. */

/* stats: auxiliary library for analyzing sample plaintexts, and
   judging plaintext "likelihood". */

#ifndef STATS_H
#define STATS_H

#define STATS_EFORMAT ((void *)(-1))

struct stats_s;
typedef struct stats_s stats_t;

stats_t *stats_new();
void stats_digest(FILE *fin, stats_t *t);
void stats_write(FILE *fout, stats_t *t);
void stats_free(stats_t *t);

stats_t *stats_read(FILE *fin);
double stats_A(stats_t *t);
double estimate_block(int block[256], int n, stats_t *t, double A);


#endif /* STATS_H */
