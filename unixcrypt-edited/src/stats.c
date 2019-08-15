/* Copyright (C) 2006-2013 Peter Selinger.  
   This file is part of unixcrypt-breaker. It is free software and it
   is distributed under the terms of the GNU general public license.
   There is no warranty. See the file COPYING for details. */

/* library to deal with statistical analysis of texts (estimate
   likelihood of plaintexts) */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stats.h"

/* a stats table holds information about letter counts and n-span
   counts. An n-span is a pair of letters that is n positions apart.
   For example, the word "example" contains the 2-spans (e,a), (x,m),
   (a,p) and so forth. A 1-span is the same as a bigram. The freq
   components reflect the same information, but as relative (and
   possibly biased) frequencies rather than counts. */
struct stats_s {
  int maxn;                         /* number of components in count2 */
  unsigned int count1[256];         /* single letter counts */
  unsigned int count1_total;        /* single letter total */
  unsigned int (*count2)[256][256]; /* n-span counts */
  unsigned int *count2_total;       /* n-span totals */
  double freq1[256];                /* single letter frequencies */
  double (*freq2)[256][256];        /* n-span conditional frequencies */
};

/* allocate a new empty stats table. Return NULL on error with errno set. */
stats_t *stats_new() {
  stats_t *t;
  int maxn = 5;

  /* allocate */
  t = (stats_t *)malloc(sizeof(stats_t));
  if (!t) {
    return NULL;
  }
  t->count2 = (unsigned int (*)[256][256])malloc(maxn * sizeof(unsigned int [256][256]));
  if (!t->count2) {
    free(t);
    return NULL;
  }
  t->count2_total = (unsigned int *)malloc(maxn * sizeof(unsigned int));
  if (!t->count2_total) {
    free(t->count2);
    free(t);
    return NULL;
  }
  t->freq2 = (double (*)[256][256])malloc(maxn * sizeof(double [256][256]));
  if (!t->freq2) {
    free(t->count2_total);
    free(t->count2);
    free(t);
    return NULL;
  }

  /* initialize */
  t->maxn = maxn;
  memset(t->count1, 0, sizeof(t->count1));
  memset(t->count2, 0, maxn * sizeof(*t->count2));
  t->count1_total = 0;
  memset(t->count2_total, 0, maxn * sizeof(*t->count2_total));
  memset(t->freq1, 0, sizeof(t->freq1));
  memset(t->freq2, 0, maxn * sizeof(*t->freq2));

  return t;
};

/* free a stats table */
void stats_free(stats_t *t) {
  if (t) {
    free(t->count2);
    free(t->count2_total);
    free(t->freq2);
  }
  free(t);
}

/* add the statistics from corpus file fin to the stats table */
void stats_digest(FILE *fin, stats_t *t) {
  int maxn = t->maxn;
  int c;
  int cc[maxn];
  int i;

  for (i=0; i<maxn; i++) {
    cc[i] = -1;
  }
  while (1) {
    c = fgetc(fin);
    if (c == EOF || c<0 || c>255) {
      break;
    }
    t->count1[c]++;
    t->count1_total++;
    for (i=1; i<maxn; i++) {
      if (cc[i] != -1) {
	t->count2[maxn-i][cc[i]][c]++;
	t->count2_total[maxn-i]++;
      }
    }
    for (i=0; i<maxn-1; i++) {
      cc[i] = cc[i+1];
    }
    cc[maxn-1] = c;
  }
}

/* fill in freq fields of a stats table. The "bias" determines how
   many times each character is assumed to have occurred a priori. */
static void stats_freq(stats_t *t, double bias) {
  int i, j, k;

  for (i=0; i<256; i++) {
    t->freq1[i] = (t->count1[i]+bias) / (t->count1_total+256*bias);
  }
  memset(t->freq2, 0, t->maxn * sizeof(*t->freq2));
  for (j=1; j<t->maxn; j++) {
    for (i=0; i<256; i++) {
      if (t->freq1[i] != 0.0) {
	for (k=0; k<256; k++) {
	  t->freq2[j][i][k] = (t->count2[j][i][k]+bias/256) / (t->count2_total[j]+256*bias) / t->freq1[i];
	}
      } else {
	for (k=0; k<256; k++) {
	  t->freq2[j][i][k] = t->freq1[k];
	}
      }
    }
  }
}

#define FORMAT_VERSION 1

/* write stats table to a file in machine-readable format. Note: no
   need to write the derived freq information. */
void stats_write(FILE *fout, stats_t *t) {
  int i, j, k;
  fprintf(fout, "# stats file for "PACKAGE_STRING"\n");
  fprintf(fout, "v %d\n\n", FORMAT_VERSION);

  fprintf(fout, "X %d\n", t->count1_total);
  for (i=0; i<256; i++) {
    if (t->count1[i] != 0) {
      fprintf(fout, "x %02x %d\n", i, t->count1[i]);
    }
  }
  
  for (j=1; j<t->maxn; j++) {
    fprintf(fout, "Y %d %d\n", j, t->count2_total[j]);
    for (i=0; i<256; i++) {
      for (k=0; k<256; k++) {
	if (t->count2[j][i][k] != 0) {
          fprintf(fout, "y %d %02x %02x %d\n", j, i, k, t->count2[j][i][k]);
        }
      }
    }
  }
}

/* read new stats object from file, as written by stats_write. Also
   supply derived frequency information. Return NULL with errno set on
   generic error, or return STATS_EFORMAT if a badly formatted file is
   encountered. */
stats_t *stats_read(FILE *fin) {
  char line[80];
  char *p;
  unsigned int n;
  int c, d, j, r;
  stats_t *t = NULL;
  int version_check = 0;

  t = stats_new();
  if (!t) {
    return NULL;
  }

  while (1) {
    p = fgets(line, sizeof(line), fin);
    if (p == NULL) {
      break;
    }
    switch (line[0]) {
    case '#': case '\0': case '\n':
      continue;
    case 'v':
      r = sscanf(line, "v %u", &n);
      if (r != 1) {
	goto error;
      }
      if (n != FORMAT_VERSION) {
	goto error;
      }
      version_check = 1;
      break;
    case 'X':
      r = sscanf(line, "X %u", &n);
      if (r != 1) {
	goto error;
      }
      t->count1_total = n;
      break;
    case 'x':
      r = sscanf(line, "x %x %u", &c, &n);
      if (r != 2 || c<0 || c>255) {
	goto error;
      }
      t->count1[c] = n;
      break;
    case 'Y':
      r = sscanf(line, "Y %d %u", &j, &n);
      if (r != 2 || j<1) {
	goto error;
      } else if (j>=t->maxn) {
	continue;
      }
      t->count2_total[j] = n;
      break;
    case 'y':
      r = sscanf(line, "y %d %x %x %u", &j, &c, &d, &n);
      if (r != 4 || j<1 || c<0 || c>255 || d<0 || d>255) {
	goto error;
      } else if (j>=t->maxn) {
	continue;
      }
      t->count2[j][c][d] = n;
      break;
    default:
      goto error;
    }
  }
  if (!version_check) {
    goto error;
  }

  /* note: we set the bias to 0; even a small bias leads to a great
     slowdown and much worse accuracy. */
  stats_freq(t, 0.0000);
  return t;

 error:
  stats_free(t);
  return STATS_EFORMAT;
}

/* ---------------------------------------------------------------------- */

/* calculate average single letter frequency */
double stats_A(stats_t *t) {
  double A = 0.0;
  int i;

  for (i=0; i<256; i++) {
    A += t->freq1[i] * t->freq1[i];
  }
  return A;
}

/* calculate the average "likelihood" of the given n-byte partial
   block, using frequency model t. Missing characters are marked -1. A
   is the "single-letter average probability", or 1 to calculate
   absolute likelihood. The whole probability is re-normalized by
   multiplying by A^256, to avoid underflow errors. */
double estimate_block(int *block, int n, stats_t *t, double A) {
  double p = 1.0; /* probability */
  int i0, c0, i, c;

  /* we do a simple "walk" of the block, keeping track of probability
     of pairs of consecutive non-blank characters. */

  i0 = -1;   /* position of previous non-blank character */
  c0 = 0;    /* value of previous non-blank character */
  for (i=0; i<n; i++) {
    if (block[i] != -1) {
      c = block[i];
      if (i0 != -1 && i-i0 < t->maxn) {
	p *= t->freq2[i-i0][c0][c];
      } else {
	p *= t->freq1[c];
      }
      p /= A;
      i0 = i;
      c0 = c;
    }
  }
  return p;
}
