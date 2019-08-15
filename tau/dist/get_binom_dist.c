#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <gmp.h>

#define DIST_LENGTH 20

static inline unsigned int choose_2(unsigned int n) {
	return n*(n-1)/2;
}
static inline unsigned int choose(int n, int k) {
	unsigned int acc = 1;
	if(n-k<k)
		k = n-k;
	int nmk = n-k;
	for(;n>nmk;n--)
		acc *= n;
	for(;k>0;k--)
		acc /= k;
	return acc;
}
static void binom(int n_sigs, double p, unsigned int k) {
	unsigned int n = choose_2(n_sigs);
	//return choose(n, k)*pow(p, k)*pow(1-p, n-k);
	mpf_t m_p, m_omp;
	mpf_init_set_d(m_p, p);
	mpf_init_set_d(m_omp, 1-p);
	mpf_pow_ui(m_p, m_p, k);
	mpf_pow_ui(m_omp, m_omp, n-k);
	mpf_mul(m_p, m_p, m_omp);
	mpf_mul_ui(m_p, m_p, choose(n, k));
	mpf_out_str(stdout, 10, 10, m_p);
	//return &m_p;
}

int main(int argc, char *argv[]) {
	unsigned int i;
	if(argc <= 1) {
		fprintf(stderr, "not enough arguments\n");
		return 1;
	}
	int number_of_sigmas = strtol(argv[1], NULL, 10);

	// init gmp
	mpf_set_default_prec(256);

	for(i=0;i<DIST_LENGTH;i++) {
		//printf("%lf\t", binom(number_of_sigmas, 1.0/65536.0, i));
		binom(number_of_sigmas, 1.0/65536.0, i);
		printf("\t");
	}
	printf("\n");
}
