/* Copyright (C) 2006-2013 Peter Selinger.  
   This file is part of unixcrypt-breaker. It is free software and it
   is distributed under the terms of the GNU general public license.
   There is no warranty. See the file COPYING for details. */

/* block processing functions */

#ifndef BLOCK_H
#define BLOCK_H

#define DISPLAY_NONE 0
#define DISPLAY_MONO 1
#define DISPLAY_COLOR 2
#define DISPLAY_SCROLL 3

int read_block(FILE *fin, int block[256]);
void unmask_block(int n, int block[256]);
void erase_block(int block[256]);
void process_block(int N, stats_t **fr_x, int *n_x, int (*block_cipher_x)[256], int (*block_plain_x)[256], int wires_known[256], int display_mode, int debug, int b, int display_max);
void write_block_hex(FILE *fout, int n, int block[256], double confidence[256], int color);
void write_block(FILE *fout, int n, int block[256]);
void write_wires_long(FILE *fout, int wires_known[256], int b);

#endif /* BLOCK_H */

