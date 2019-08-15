#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <gmp.h>


// TODO REMOVE!
/*static inline int choose(int n, int k) {
	//fprintf(stderr, "n: %d\t k:%d\n", n, k);
	//return fac(n)/(fac(k)*fac(n-k));
	// TODO REIMPLEMENT WITHOUT FAC AND CANCELLING TOP BOTTOM
	double acc = 1;
	if(n-k<k)
		k = n-k;
	int nmk = n-k;
	for(;n>nmk;n--)
		acc *= n;
	for(;k>0;k--)
		acc /= k;
	return acc;
}

static inline double boolean_pdf(int x, int n, double p) {
	return choose(n,x)*pow(p,x)*pow(1-p,n-x);
}*/

int main(int argc, char *argv[]) {
	int i, j;
	char c;
	// INPUT SUMBER OF SIGMAS SAMPLED
	/*if(argc<2) {
		fprintf(stderr, "not enough arguments\n");
		return 1;
	}*/


	int wire_start = -1, number_of_sigmas = -1, table_width = -1;
	fscanf(stdin, "%*s %*s %*s %*s %d", &wire_start); // "data for wire from %d"
	fscanf(stdin, "%*s %*s %d %*s", &number_of_sigmas); // "sampled from %d sigmas"
	fscanf(stdin, "%*s %*s %*s"); // "to \ coincidences"
	while(fscanf(stdin, "%d%c ", &table_width, &c)==2 && c!='\n');
	table_width++; // while loop counts highest label, but 0 must also be included

	// GENERATE DISTRIBUTION APPROXIMATIONS FOR CORRECT AND INCORRECT WIRES
	double *correct = NULL;
	       //*incorrect = NULL;
	correct = malloc(table_width*sizeof(double));
	//incorrect = malloc(table_width*sizeof(double));
	//int number_of_pairs = choose_2(number_of_sigmas);


	/*for(i=0;i<table_width;i++) {
		//*(incorrect+i) = boolean_pdf(i, number_of_pairs, 1.0/65536.0);
		*(correct+i) = (255.0l/64771.0l)*boolean_pdf(i, number_of_pairs, 1.0l/256.0l)+
		               (64516.0l/64771.0l)*boolean_pdf(i, number_of_pairs, 1.0l/65536.0l);
		//*(correct+i) = boolean_pdf(i, number_of_pairs, 1.5258789062E-5);
		// TODO WHY DID IT WORK WITH THIS??
	}*/
	/*for(i=0;i<table_width;i++)
		*(correct+i) = pow(10,i);*/
	
	// READ DISTRIBUTION FROM A FILE
	if(argc<2) {
		fprintf(stderr, "Not enough arguments. You must supply a distribution file.\n");
		return 1;
	}
	FILE *dist = fopen(argv[1], "r");
	//fprintf(stderr, "file: %s\n", argv[1]);
	for(i=0;i<table_width;i++) {
		if(fscanf(dist,"%lf ",correct+i)<1) {
			fprintf(stderr, "Error reading distribution file");
			return 1;
		}
	}
	//for(i=0;i<table_width;i++)
	//	fprintf(stderr, "%lf, ", *(correct+i));
	//fprintf(stderr, "\n");


	// INPUT K TABLE (sigmas_to_tau)
	// CALCULATE LIKELIHOOD FOR EACH WIRE
	mpf_set_default_prec(256);
	mpf_t likelihood[256];
	for(i=0;i<256;i++)
		mpf_init_set_d(likelihood[i], 1);
	mpf_t tmp;
	mpf_init(tmp);
	unsigned int curr;
	for(i=0;i<256;i++) { // row i
		// TODO SKIP ROW FOR WIRE BEING READ wire_start
		scanf("%*d");
		for(j=0;j<table_width;j++) {
			scanf("%u",&curr);
			//likelihood[i] *= pow(*(correct+j), curr); // TODO CURR IS TOO HIGH SO IT ALWAYS GOES TO 0!
			if(curr>0 && *(correct+j)==0) {
				fprintf(stderr, "sampling from 0 distribution on likelihood[%d]!\n", i);
				//return 1;
			}
			mpf_set_d(tmp, *(correct+j));
			mpf_pow_ui(tmp, tmp, curr);
			mpf_mul(likelihood[i], likelihood[i], tmp);
		}
		//fprintf(stderr, "likelihood[%d]\t", i);
		//mpf_out_str(stderr, 10, 10, likelihood[i]);
		//fprintf(stderr, "\n");
	}

	// SELECT MIN LIKELIHOOD
	mpf_t max;
	mpf_init_set_d(max, 1);
	int max_i = -1;
	for(i=0;i<256;i++) {
		//if(likelihood[i]>max) {
		// TODO CHANGE THIS TO USE wire_start!
		if(mpf_cmp_d(likelihood[i], 1) && mpf_cmp(likelihood[i],max)<0) {
			//max = likelihood[i];
			mpf_swap(max,likelihood[i]);
			max_i = i;
		}
	}

	// OUTPUT
	fprintf(stderr, "best i: %d\nlikelihood ",max_i);
	mpf_out_str(stderr, 10, 10, max);
	fprintf(stderr, "\n");

	fprintf(stdout, "%d\n", max_i);
	// CLEANUP
	free(correct);
	fclose(dist);
	mpf_clear(tmp);
	for(i=0;i<256;i++)
		mpf_clear(likelihood[i]);
	return 0;
}
