/* Copyright (C) 2006-2013 Peter Selinger.  
   This file is part of unixcrypt-breaker. It is free software and it
   is distributed under the terms of the GNU general public license.
   There is no warranty. See the file COPYING for details. */

/* ---------------------------------------------------------------------- */
/* user interface */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "stats.h"
#include "block.h"

#define NAME "unixcrypt-breaker"

/* ---------------------------------------------------------------------- */
/* print usage information */

static void usage(FILE *fout) {
  fprintf(fout, ""NAME" "VERSION". Break unix \"crypt\" encryption.\n\n");
  fprintf(fout, 
  "Usage: "NAME" [options] -s <statfile> -c <corpusfile>...\n"
  "       "NAME" [options] -s <statfile> <cipherfile>...\n"
  "\n"
  "The code-breaking algorithm relies on statistical information from a\n"
  "corpus of sample data that is presumed similar to the unknown\n"
  "plaintext. To use this program, you must first run "NAME"\n"
  "with the -c option on a corpus of sample data to create a statfile\n"
  "(see the first form of usage above). You may then use the resulting\n"
  "statfile to attempt decryption of the cipherfile (second form).\n"
  "\n"
  "Options:\n"
  " -h, --help            print this help message and exit\n"
  " -V, --version         print version info and exit\n"
  " -L, --license         print license info and exit\n"
  " -D, --debug           raise debugging level\n"
  " -c, --corpus          create statfile from one or more files of sample\n"
  "                       data, or from stdin if no filenames are given.\n"
  " -s, --statfile <file> specify path to statfile (mandatory)\n"
  " -d, --display <mode>  progress display: color (default), mono, scroll, or none\n"
  " -m, --maxdisplay <n>  how many blocks fit on the display (default 1)\n"
  " -o, --output <file>   select output file (default: <cipherfile>.guess)\n"
  " -w, --wirefile <file> output wiring information (advanced) to file\n"
  " --                    end of options, filenames follow\n");
}

/* print version and copyright information */
static void version(FILE *fout) {
  fprintf(fout, ""NAME" "VERSION". Break unix \"crypt\" encryption.\n");
  fprintf(fout, "Copyright (C) 2006-2013 Peter Selinger.\n");
}

static void license(FILE *fout) {
  fprintf(fout, ""NAME" "VERSION". Break unix \"crypt\" encryption.\n");
  fprintf(fout, "Copyright (C) 2006-2013 Peter Selinger.\n");
  fprintf(fout, "\n");
  fprintf(fout,
  "This program is free software; you can redistribute it and/or modify\n"
  "it under the terms of the GNU General Public License as published by\n"
  "the Free Software Foundation; either version 2 of the License, or\n"
  "(at your option) any later version.\n"
  "\n"
  "This program is distributed in the hope that it will be useful,\n"
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
  "GNU General Public License for more details.\n"
  "\n"
  "You should have received a copy of the GNU General Public License\n"
  "along with this program; if not, see <http://www.gnu.org/licenses/>.\n"
  );
}

static struct option longopts[] = {
  {"help",         0, 0, 'h'},
  {"version",      0, 0, 'V'},
  {"license",      0, 0, 'L'},
  {"corpus",       0, 0, 'c'},
  {"statfile",     1, 0, 's'},
  {"output",       1, 0, 'o'},
  {"display",      1, 0, 'd'},
  {"maxdisplay",   1, 0, 'm'},
  {"debug",        0, 0, 'D'},
  {"wirefile",     1, 0, 'w'},
  {0, 0, 0, 0}
};

static char *shortopts = "-hVLcs:o:d:Dw:m:";

static int digest_corpus(char *statfile, char **av, int ac) {
  FILE *fstat;
  FILE *fin;
  int i;
  stats_t *t;
  
  fstat = fopen(statfile, "w");
  if (!fstat) {
    fprintf(stderr, ""NAME": %s: %s\n", statfile, strerror(errno));
    exit(1);
  }      

  t = stats_new();
  if (!t) {
    fprintf(stderr, ""NAME": %s\n", strerror(errno));
    exit(1);
  }
  
  if (ac == 0) {  /* read from stdin */
    stats_digest(stdin, t);
  } else {
    for (i=0; i<ac; i++) {
      fin = fopen(av[i], "r");
      if (!fin) {
	fprintf(stderr, ""NAME": %s: %s, skipping\n", av[i], strerror(errno));
      } else {
	stats_digest(fin, t);
	fclose(fin);
      }
    }
  }
  stats_write(fstat, t);
  stats_free(t);
  fclose(fstat);
  return 0;
}

/* Read N streams of ciphertext from fcipher_x[], and write N streams
   of plaintext guesses to fout_x[]. Display progress info according
   to display_mode and debug. fwire, if non-NULL, is a stream to write
   wiring info to. fr_x[] is an array of statistics. Display_max
   specifies how many blocks can be simultaneously displayed on the
   screen. */
static void unixcrypt_breaker_streams(int N, FILE **fcipher_x, FILE *fwire, FILE **fout_x, stats_t **fr_x, int display_mode, int debug, int display_max) {
  int b, j;
  int block_cipher_x[N][256];
  int block_plain_x[N][256];
  int wires_known[256];
  int n_x[N];
  int file_ended_x[N];  /* has file j ended? */
  int nopen;            /* number of non-ended files */

  nopen = N;
  memset(file_ended_x, 0, sizeof(file_ended_x));

  b=0;
  while (1) {
    for (j=0; j<N; j++) {
      if (!file_ended_x[j]) {
	n_x[j] = read_block(fcipher_x[j], block_cipher_x[j]);
      } else {
	n_x[j] = 0;
      }
    }
    for (j=0; j<N; j++) {
      unmask_block(n_x[j], block_cipher_x[j]);
      erase_block(block_plain_x[j]);
    }
    process_block(N, fr_x, n_x, block_cipher_x, block_plain_x, wires_known, display_mode, debug, b, display_max);
    if (display_mode == DISPLAY_SCROLL) {
      for (j=0; j<N; j++) {
	if (n_x[j] != 0) {
	  fprintf(stdout, "File %d, block %d:\n", j, b);
	  write_block_hex(stdout, n_x[j], block_plain_x[j], NULL, 0);
	  fprintf(stdout, "\n");
	}
      }
    }
    for (j=0; j<N; j++) {
      if (n_x[j] != 0) {
	write_block(fout_x[j], n_x[j], block_plain_x[j]);
	fflush(fout_x[j]);
      }
    }
    if (fwire) {
      write_wires_long(fwire, wires_known, b);
      fflush(fwire);
    }
    for (j=0; j<N; j++) {
      if (!file_ended_x[j] && n_x[j] < 256) {
	file_ended_x[j] = 1;
	nopen--;
      }
    }
    if (nopen == 0) {
      break;
    }
    b++;
  }
}

/* Decipher N files from cipherfile_x[] to outfile_x[], using
   statistics files statfile_x[]. All files must be encrypted with the
   same key. wirefile is optional; if present, wiring information will
   be written to it. Print progress and debug info according to
   display_mode and debug. Display_max specifies how many blocks can
   be simultaneously displayed on the screen. */
static int unixcrypt_breaker(int N, char **cipherfile_x, char **statfile_x, char *wirefile, char **outfile_x, int display_mode, int debug, int display_max) {
  int j;

  FILE *fcipher_x[N];
  FILE *fwire;
  FILE *fstat;
  FILE *fout_x[N];
  stats_t *fr_x[N];

  /* open and read statfiles. */
  for (j=0; j<N; j++) {
    /* optimize in case the same statfile is repeated */
    if (j>0 && strcmp(statfile_x[j], statfile_x[j-1]) == 0) {
      fr_x[j] = fr_x[j-1];
      continue;
    }
    fstat = fopen(statfile_x[j], "r");
    if (!fstat) {
      fprintf(stderr, ""NAME": %s: %s\n", statfile_x[j], strerror(errno));
      exit(1);
    }      
    fr_x[j] = stats_read(fstat);
    if (!fr_x[j]) {
      fprintf(stderr, ""NAME": %s\n", strerror(errno));
      return 1;
    } else if (fr_x[j] == STATS_EFORMAT) {
      fprintf(stderr, ""NAME": %s: bad file format\n", statfile_x[j]);
      return 1;
    }
    fclose(fstat);
  }

  /* open cipherfiles */
  for (j=0; j<N; j++) {
    fcipher_x[j] = fopen(cipherfile_x[j], "r");
    if (!fcipher_x[j]) {
      fprintf(stderr, ""NAME": %s: %s\n", cipherfile_x[j], strerror(errno));
      exit(1);
    }
  }
  
  /* open outfiles */
  for (j=0; j<N; j++) {
    fout_x[j] = fopen(outfile_x[j], "w");
    if (!fout_x[j]) {
      fprintf(stderr, ""NAME": %s: %s\n", outfile_x[j], strerror(errno));
      exit(1);
    }
  }

  /* open wirefile, if any */
  if (wirefile) {
    fwire = fopen(wirefile, "w");
    if (!fwire) {
      fprintf(stderr, ""NAME": %s: %s\n", wirefile, strerror(errno));
      exit(1);
    }
  } else {
    fwire = NULL;
  }

  unixcrypt_breaker_streams(N, fcipher_x, fwire, fout_x, fr_x, display_mode, debug, display_max);

  for (j=0; j<N; j++) {
    fclose(fcipher_x[j]);
    fclose(fout_x[j]);
  }
  if (fwire) {
    fclose(fwire);
  }
  return 0;
}

int main(int ac, char *av[]) {
  int c, j;
  const char *guessext = ".guess";
  char *statfile_y = NULL;
  char *outfile_y = NULL;

  /* command line options and defaults */
  int display_mode = DISPLAY_COLOR;  
  int corpus_mode = 0;   /* 1=on, 0=off */
  char *wirefile = NULL;
  char **outfile_x = NULL;
  char **cipherfile_x = NULL;
  char **statfile_x = NULL;
  int N = 0; /* current size of outfile_x, cipherfile_x, statfile_x arrays */
  int debug = 0;
  int display_max = 1;

  while ((c = getopt_long(ac, av, shortopts, longopts, NULL)) != -1) {
    switch (c) {
    case 'h':
      usage(stdout);
      exit(0);
      break;
    case 'V':
      version(stdout);
      exit(0);
      break;
    case 'L':
      license(stdout);
      exit(0);
      break;
    case 'c':
      corpus_mode = 1;
      break;
    case 1:
      N++;
      cipherfile_x = realloc(cipherfile_x, N * sizeof(*cipherfile_x));
      if (!cipherfile_x) {
	goto error;
      }
      statfile_x = realloc(statfile_x, N * sizeof(*statfile_x));
      if (!statfile_x) {
	goto error;
      }
      outfile_x = realloc(outfile_x, N * sizeof(*outfile_x));
      if (!outfile_x) {
	goto error;
      }
      statfile_x[N-1] = statfile_y;
      cipherfile_x[N-1] = optarg;
      outfile_x[N-1] = outfile_y;
      outfile_y = NULL;
      break;
    case 's':
      statfile_y = optarg;
      for (j=0; j<N; j++) {
	if (statfile_x[j] == NULL) {
	  statfile_x[j] = statfile_y;
	} else {
	  break;
	}
      }
      break;
    case 'o':
      outfile_y = optarg;
      break;
    case 'd':
      if (strcmp(optarg, "color") == 0) {
	display_mode = DISPLAY_COLOR;
      } else if (strcmp(optarg, "mono") == 0) {
	display_mode = DISPLAY_MONO;
      } else if (strcmp(optarg, "scroll") == 0) {
	display_mode = DISPLAY_SCROLL;
      } else if (strcmp(optarg, "none") == 0) {
	display_mode = DISPLAY_NONE;
      } else {
	fprintf(stderr, ""NAME": invalid display mode: %s. Try --help for more info.\n", optarg);
	exit(1);
      }
      break;
    case 'm':
      display_max = atoi(optarg);
      if (display_max < 1) {
	fprintf(stderr, ""NAME": argument to -m must be 1 or more\n");
	exit(1);
      }
      break;
    case 'D':
      debug++;
      break;
    case 'w':
      wirefile = optarg;
      break;
    case '?': default:
      fprintf(stderr, "Try --help for more information.\n");
      exit(1);
      break;
    }
  }

  if (N == 0) {
    fprintf(stderr, ""NAME": no filenames specified. Try --help for more info.\n");
    exit(1);
  }

  /* check for statfile */
  if (statfile_y == NULL) {
    fprintf(stderr, ""NAME": the -s option is mandatory. Try --help for more info.\n");
    exit(1);
  }

  if (corpus_mode) {
    return digest_corpus(statfile_y, cipherfile_x, N);
  }

  /* check for unused outfile */
  if (outfile_y && N > 1) {
    fprintf(stderr, ""NAME": -o option at the end of file list not allowed\n");
    exit(1);
  } else if (outfile_y && N == 1) {  /* for backward compatibility */
    outfile_x[0] = outfile_y;
  }

  for (j=0; j<N; j++) {
    if (outfile_x[j] == NULL) {
      outfile_x[j] = malloc(strlen(cipherfile_x[j])+strlen(guessext)+1);
      if (!outfile_x[j]) {
	goto error;
      }
      strcpy(outfile_x[j], cipherfile_x[j]);
      strcat(outfile_x[j], guessext);
    }
  }

  return unixcrypt_breaker(N, cipherfile_x, statfile_x, wirefile, outfile_x, display_mode, debug, display_max);

 error:
  fprintf(stderr, ""NAME": %s\n", strerror(errno));
  exit(1);
}
