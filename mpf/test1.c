#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gmp.h>

int main(int argc, char *argv[]) {
	// set default precision
	mpf_set_default_prec(512);
	// init var
	mpf_t x, y;
	mpf_init(x);
	mpf_init(y);
	// set
	mpf_set_d(x, 3);
	mpf_set_d(y, 5.003);
	// TODO USE ...init_set... functions
	// calculate
	mpf_add(x,x,y);
	mpf_out_str(stdout, 10, 20, x);
	printf("\n");
	// free var
	mpf_clear(x);
	return 0;
}
