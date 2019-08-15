/* Copyright (C) 2006-2013 Peter Selinger.  
   This file is part of unixcrypt-breaker. It is free software and it
   is distributed under the terms of the GNU general public license.
   There is no warranty. See the file COPYING for details. */

/* block processing functions */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "stats.h"
#include "block.h"

/* ---------------------------------------------------------------------- */
/* auxiliary: various i/o */

/* terminal specific codes (here: color xterm) */
#define HOMEKEY      "\033[H"
#define SCROLLKEY    "\033[S"
#define CLEARKEY     "\033[24S\033[H\033[J"
#define COLORBLACK   "\033[30m"
#define COLORRED     "\033[31m"
#define COLORGREEN   "\033[32m"
#define COLORYELLOW  "\033[33m"
#define COLORBLUE    "\033[34m"
#define COLORMAGENTA "\033[35m"
#define COLORCYAN    "\033[36m"
#define COLORWHITE   "\033[37m"

/* assign color code to confidence */
static char *color_code(double confidence) {
  if (confidence >= 0.5) {
    return COLORBLACK;
  } else if (confidence >= 0.2) {
    return COLORBLUE;
  } else if (confidence >= 0.1) {
    return COLORGREEN;
  } else if (confidence >= 0.05) {
    return COLORRED;
  } else {
    return COLORYELLOW;
  }
}

static int is_printable(int x) {
  return x>=32 && x<=126;
}

static int to_printable(int x) {
  return is_printable(x) ? x : '#';
}

/* read 256 bytes from fin. Return number of bytes read. */
int read_block(FILE *fin, int block[256]) {
  int i, c;

  memset(block, 0, 256*sizeof(int));

  for (i=0; i<256; i++) {
    c = fgetc(fin);
    if (c == EOF) {
      break;
    }
    block[i] = c & 0xff;
  }
  return i;
}

/* write the given block in hex and ASCII, replacing -1 by empty
   character. If color != 0, use colors according to confidence
   matrix.  */
void write_block_hex(FILE *fout, int n, int block[256], double confidence[256], int color) {
  int i, j;
  int c;

  fprintf(fout, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f     0123456789abcdef\n");
  for (i=0; i<256; i+=16) {
    if (i > n) {
      break;
    }
    fprintf(fout, "%01x:  ", i >> 4);
    for (j=0; j<16; j++) {
      if (i+j >= n) {
	fprintf(fout, "** ");
      } else if (block[i+j] != -1) {
	if (color) {
	  c = (block[i+j] + i+j) & 0xff;
	  fprintf(fout, "%s", color_code(confidence[c]));
	}
	fprintf(fout, "%02x ", block[i+j]);
	if (color) {
	  fprintf(fout, ""COLORBLACK"");
	}
      } else {
	fprintf(fout, "-- ");
      }
    }
    fprintf(fout, " %01x: ", i >> 4);
    for (j=0; j<16; j++) {
      if (i+j >= n) {
	fprintf(fout, "*");
      } else if (block[i+j] == -1) {
	fprintf(fout, ".");
      } else {
	if (color) {
	  c = (block[i+j] + i+j) & 0xff;
	  fprintf(fout, "%s", color_code(confidence[c]));
	}
	fprintf(fout, "%c", to_printable(block[i+j]));
	if (color) {
	  fprintf(fout, ""COLORBLACK"");
	}
      }
    }
    fprintf(fout, " %01x\n", i >> 4);
  }
  fprintf(fout, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f     0123456789abcdef\n");
}

/* write the given block in ASCII, replacing -1 by " " */
void write_block(FILE *fout, int n, int block[256]) {
  int i;

  for (i=0; i<n; i++) {
    if (block[i] == -1) {
      fprintf(fout, " ");
    } else {
      fprintf(fout, "%c", block[i]);
    }
  }
}

/* write the given wiring to file. B is the block number. */
void write_wires_long(FILE *fout, int wires_known[256], int b) {
  int i, c;

  c = 0;
  fprintf(fout, "wires %d\n", b);
  for (i=0; i<256; i++) {
    if (wires_known[i] != -1) {
      fprintf(fout, "%02x-%02x ", i, wires_known[i]);
      c++;
      if (c==8) {
	c=0;
	fprintf(fout, "\n");
      }
    }
  }
  if (c) {
    fprintf(fout, "\n");
  }
  fprintf(fout, "end wires\n\n");
}

/* ---------------------------------------------------------------------- */
/* auxiliary: algorithm */

/* unmask the given block. */
void unmask_block(int n, int block[256]) {
  int i, c;

  for (i=0; i<n; i++) {
    c = block[i];
    block[i] = (c+i) & 0xff;
  }
}

void erase_block(int block[256]) {
  int i;
  
  for (i=0; i<256; i++) {
    block[i] = -1;
  }
}

#if 0
static void block_to_wires(int block_cipher[256], int block_plain[256], int wires_known[256]) {
  int i, x, b;

  for (i=0; i<256; i++) {
    if (block_plain[i] != -1) {
      x = block_cipher[i];
      b = (block_plain[i] + i) & 0xff;
      wires_known[x] = b;
      wires_known[b] = x;
    }
  }
}
#endif

/* add decoding of a single (directed) wire x->y to the partial
   plaintext block. */
static void add_wire_directed(int block_cipher[256], int x, int y, int block_plain[256]) {
  int i;
  
  for (i=0; i<256; i++) {
    if (block_cipher[i] == x) {
      block_plain[i] = (y-i) & 0xff;
    }
  }
}

/* partially decipher (unmasked) block according to partial wiring.
   Note: existing plaintext is not erased! */
static void wires_to_block(int block_cipher[256], int wires_known[256], int block_plain[256]) {
  int i, x;

  for (i=0; i<256; i++) {
    x = block_cipher[i];
    if (wires_known[x] != -1) {
      block_plain[i] = (wires_known[x] - i) & 0xff;
    }
  }
}

/* ---------------------------------------------------------------------- */
/* main algorithm */

/* fill prob[][] from the given N cipher blocks (all encrypted with
   the same wiring). Input information about ngram stats, and known
   wires and non-wires. Block_guess may be used as an additional
   guess, to bias probabilities, but is not taken as an absolute
   truth. n_x[j] is the size of the jth block. */
static void probability_matrix(double prob[256][256], int N, stats_t **fr_x, double *A_x, int *n_x, int (*block_cipher_x)[256], int wires_known[256], char no_wires[256][256], int (*block_guess_x)[256], int debug) {
  int x, i, b, j;
  double p, q, B;
  double prob_total;
  int block_plain_x[N][256];
  int block_plain1[256];

  memset(prob, 0, 256*256*sizeof(double));
  for (j=0; j<N; j++) {
    erase_block(block_plain_x[j]);
    wires_to_block(block_cipher_x[j], wires_known, block_plain_x[j]);
  }

  /* process each repeated code */
  for (x=0; x<256; x++) {
    if (wires_known[x] != -1) {
      continue;
    }

    /* figure out relative probabilities of each wire */
    prob_total = 0.0;
    for (b=0; b<256; b++) {
      /* wire x <-> b */
      if (no_wires[x][b] || wires_known[b] != -1) {
	continue;
      }

      p = 1.0;
      for (j=0; j<N; j++) {
	if (n_x[j] > 0) {
	  memcpy(block_plain1, block_plain_x[j], sizeof(block_plain1));
	  add_wire_directed(block_cipher_x[j], x, b, block_plain1);
	  p *= estimate_block(block_plain1, n_x[j], fr_x[j], A_x[j]);
	}
      }
      prob[x][b] = p;
      prob_total += p;
    }
    /* figure out the absolute probability of each wire */
    for (b=0; b<256; b++) {
      if (no_wires[x][b]) {
	continue;
      }
      prob[x][b] /= prob_total;
      if (debug > 1 && prob[x][b] > 0.05) {
	fprintf(stderr, "Wire %02x-%02x: Prob = %.10f\n", x, b, prob[x][b]);
      }
    }
  }
  /* now calculate Bayesian probability for each wire */
  for (x=0; x<256; x++) {
    if (wires_known[x] != -1) {
      continue;
    }
    for (b=x+1; b<256; b++) {
      if (no_wires[x][b] || wires_known[b] != -1) {
	continue;
      }
      p = prob[x][b];
      q = prob[b][x];
      B = (p*q) / (p*q + (1-p)*(1-q)); /* associative symmetric operation */
      if (debug > 1 && B > 0.05) {
	fprintf(stderr, "Bayes %02x-%02x: Prob = %.10f\n", x, b, B);
      }
      prob[x][b] = B;
      prob[b][x] = B;
    }
  }

  /* adjust the Bayesian probabilities by a-priori guesses */
  for (j=0; j<N; j++) {
    for (i=0; i<n_x[j]; i++) {
      if (block_guess_x[j][i] != -1) {
	x = block_cipher_x[j][i];
	b = (block_guess_x[j][i] + i) & 0xff;
	/* bias towards wire x<->b */
	if (wires_known[x] != -1 || wires_known[b] != -1 || no_wires[x][b]) {
	  continue;
	}
	p = prob[x][b];
	q = 0.99;
	B = (p*q) / (p*q + (1-p)*(1-q));
	prob[x][b] = B;
	prob[b][x] = B;
	if (debug > 2) {
	  fprintf(stderr, "Adjusting %02x-%02x: Prob %.10f -> %.10f\n", x, b, p, B);
	}

#if 0
	/* reversely bias all the conflicting options. */
	for (y=0; y<256; y++) {
	  if (wires_known[y] != -1) {
	    continue;
	  }
	  if (y != b && y != x) {
	    p = prob[x][y];
	    q = 0.01;
	    B = (p*q) / (p*q + (1-p)*(1-q));
	    prob[x][y] = B;
	    prob[y][x] = B;
	  }
	  if (y != b && y != x) {
	    p = prob[b][y];
	    q = 0.01;
	    B = (p*q) / (p*q + (1-p)*(1-q));
	    prob[b][y] = B;
	    prob[y][b] = B;
	  }
	}
#endif
      }
    }
  }
}

/* Find j0, k0 such that prob[j0][k0] is maximal among all the
   prob[j][k], where the domain of (j,k) is restricted by no_wires and
   wires_known. Returns prob[j0][k0]. */
static double maximize_prob(double prob[256][256], char no_wires[256][256], int wires_known[256], int *j0p, int *k0p) {
  int j, k, j0, k0;
  double max;

  max = -1.0;
  j0 = 0;
  k0 = 1;
  for (j=0; j<256; j++) {
    if (wires_known[j] != -1) {
      continue;
    }
    for (k=j+1; k<256; k++) {
      if (wires_known[k] != -1 || no_wires[j][k]) {
	continue;
      }
      if (prob[j][k] > max) {
	max = prob[j][k];
	j0 = j;
	k0 = k;
      }
    }
  }
  if (j0p) {
    *j0p = j0;
  }
  if (k0p) {
    *k0p = k0;
  }
  // EDITED
  //if(j0==0 && k0==1)
  //	  fprintf(stderr, "j0=0 k0=1\n");
  return max;
}

/* find the minimal non-zero probability in matrix, and also eliminate
   all wires of zero probability. */
static double minimize_prob(double prob[256][256], char no_wires[256][256], int wires_known[256], int *j0p, int *k0p) {
  double min;
  int j0, k0, j, k;

  min = 2.0;
  j0 = 0;
  k0 = 1;
  for (j=0; j<256; j++) {
    if (wires_known[j] != -1) {
      continue;
    }
    for (k=j+1; k<256; k++) {
      if (wires_known[k] != -1 || no_wires[j][k]) {
	continue;
      }
      if (prob[j][k] == 0.0) {
	no_wires[j][k] = 1;
	no_wires[k][j] = 1;
      } else if (prob[j][k] < min) {
	min = prob[j][k];
	j0 = j;
	k0 = k;
      }
    }
  }
  if (j0p) {
    *j0p = j0;
  }
  if (k0p) {
    *k0p = k0;
  }
  return min;
}

/* process the given N (unmasked) cryptographic blocks relative to the
   respective ngram statistics. All blocks must be encrypted with the
   same wiring, and each block may have its own statistics. Return
   (partial) answers in block_plain_x, and return wires in
   wires_known. n is the block size, normally 256, but sometimes
   less. Also, block_plain_x must be initialized to an initial guess
   (using -1 for unknown characters). B is the block number.
   Display_max specifies how many blocks can be simultaneously
   displayed on the screen. */
void process_block(int N, stats_t **fr_x, int *n_x, int (*block_cipher_x)[256], int (*block_plain_x)[256], int wires_known[256], int display_mode, int debug, int b, int display_max) {
  int i, j;
  double prob[256][256];
  int j0, k0;
  double max, min;
  char no_wires[256][256];
  int block_guess_x[N][256];
  double confidence[256];
  double A_x[N];
  int display_count;

  //int wires_assigned = 0; // EDITED

  for (j=0; j<N; j++) {
    A_x[j] = stats_A(fr_x[j]);
  }
  memcpy(block_guess_x, block_plain_x, N*256*sizeof(int));

  memset(no_wires, 0, 256*256*sizeof(char));
  memset(confidence, 0, 256*sizeof(double));
  for (j=0; j<N; j++) {
    erase_block(block_plain_x[j]);
  }
  for (i=0; i<256; i++) {
    no_wires[i][i] = 1;
    wires_known[i] = -1;
  }

  /* in case there is "known" plaintext */
  //block_to_wires(block_cipher, block_plain, wires_known);

  if (display_mode == DISPLAY_COLOR || display_mode == DISPLAY_MONO) {
    fprintf(stdout, ""CLEARKEY"");
  }

  for (i=0; i<128; i++) {
    if (display_mode == DISPLAY_COLOR || display_mode == DISPLAY_MONO) {
      fprintf(stdout, ""HOMEKEY"");
    }
    display_count = 0;
    for (j=0; j<N; j++) {
      if (n_x[j] > 0) {
	wires_to_block(block_cipher_x[j], wires_known, block_plain_x[j]);
	display_count++;
	if (display_count <= display_max) {
	  if (display_mode == DISPLAY_COLOR || display_mode == DISPLAY_MONO) {
	    if (display_count > 1) {
	      fprintf(stdout, "\n");
	    }
	    fprintf(stdout, "File %d, block %d:\n", j, b);
	    write_block_hex(stdout, n_x[j], block_plain_x[j], confidence, display_mode == DISPLAY_COLOR);
	  }
	}
      }
    }
    probability_matrix(prob, N, fr_x, A_x, n_x, block_cipher_x, wires_known, no_wires, block_guess_x, debug);
    
    max = maximize_prob(prob, no_wires, wires_known, &j0, &k0);

    if (max < 0.02) {
      break;
    }
    
    wires_known[j0] = k0;
    wires_known[k0] = j0;
    confidence[j0] = max;
    confidence[k0] = max;
    if (debug) {
      fprintf(stdout, "Adding wire %02x-%02x: Prob = %.10f\n", j0, k0, max);
    }
    min = minimize_prob(prob, no_wires, wires_known, &j0, &k0);
    no_wires[j0][k0] = 1;
    no_wires[k0][j0] = 1;
    if (debug) {
      fprintf(stdout, "Ruling out wire %02x-%02x: Prob = %.10f\n", j0, k0, min);
    }
  }
  /*
   * EDITED TO FILL IN WIRES
   */
  // count how many unassigned
  int unassigned_wires_count = 0;
  for(i=0;i<256;i++)
	  if(wires_known[i]==-1)
		  unassigned_wires_count++;
  fprintf(stderr, "unassigned_wures_count %d\n", unassigned_wires_count);
  // init array of that size
  int *unassigned_wires = malloc(unassigned_wires_count*sizeof(int));
  // fill array with indices of unassigned wires, keep copy

  j=0;
  for(i=0;i<unassigned_wires_count;i++) {
    while(wires_known[j]!=-1)
      j++;
    unassigned_wires[i] = j;
    j++;
  }
  
  fprintf(stderr, "before shuffle: ");
  for(i=0;i<unassigned_wires_count;i++)
    fprintf(stderr, "%d, ", unassigned_wires[i]);
  fprintf(stderr, "\n");

  // get unassigned_wires_count random bytes
  unsigned int *random_bytes = malloc(unassigned_wires_count*sizeof(int));
  FILE *urandom = fopen("/dev/urandom","r");
  fread(random_bytes, sizeof(int), unassigned_wires_count, urandom);
  fclose(urandom);
  // Fisher-Yates shuffle array
  int swp, rand;
  j=0;
  for(i=unassigned_wires_count-1;i>0;i--) {
    rand = random_bytes[j++] % (i+1); // random number 0 <= rand <= i
    swp = unassigned_wires[rand];
    unassigned_wires[rand] = unassigned_wires[i];
    unassigned_wires[i] = swp;
  }

  fprintf(stderr, "after shuffle: ");
  for(i=0;i<unassigned_wires_count;i++)
	  fprintf(stderr, "%d, ", unassigned_wires[i]);
  fprintf(stderr, "\n");


  // for all elements of copy, assign wire [copy] to [array]
  int assigned_wires = 0;
  for(i=0;i<unassigned_wires_count-1;i+=2) {
    wires_known[unassigned_wires[i]] = unassigned_wires[i+1];
    wires_known[unassigned_wires[i+1]] = unassigned_wires[i];
    assigned_wires += 2;
  }
  fprintf(stderr, "assigned %d\n", assigned_wires);
  // clean up
  free(unassigned_wires);
  free(random_bytes);
  
  /*
   * END EDITING
   */
}

