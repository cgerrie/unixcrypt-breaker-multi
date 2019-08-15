#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	int i, j, k;
	// INPUT TAU
	int tau[256] = {0};
	for(i=0;i<256;i++)
		scanf("%*x%*c%x",&tau[i]);

	/*fprintf(stderr, "tau\n");
	for(i=0;i<256;i++) {
		fprintf(stderr, "%02x-%02x ", i, tau[i]);
		if((i+1) % 8 == 0)
			fprintf(stderr, "\n");
	}*/

	// INPUT SIGMAS
	// taken from tau/K/sigmas_to_tau.c
	int n = 0,
	    *sigma = NULL; // sigma-i-hat's
	bool not_done = true;
	while(not_done) {
		sigma = realloc(sigma, (++n)*256*sizeof(int));
		fscanf(stdin, "%*s %*d\n");
		for(i=0;i<256;i++) {
			if(fscanf(stdin, "%*x-%x ", sigma+((n-1)*256+i)) != 1) {
				n--;
				not_done = false;
				break;
			}
		}
		fscanf(stdin, "%*s %*s\n");
	}
	/*fprintf(stderr, "sigma hat 2\n");
	for(i=0;i<256;i++) {
		fprintf(stderr, "%02x-%02x ", i, *(sigma+2*256+i));
		if((i+1) % 8 == 0)
			fprintf(stderr, "\n");
	}*/

	// CALCULATE TAU CONJUGATE OF SIGMA I HAT
	int tau_inv[256] = {0},
	    tau_tti[256] = {0}, // tau-to-the-i
	    tau_inv_tti[256] = {0}, // tau-inverse-to-the-i
	    tmp[256] = {0},
	    *sigma_conj = NULL;
	sigma_conj = malloc(n*256*sizeof(int));
	// calculate tau_inv
	// TODO NON O(N^2) ALG
	for(i=0;i<256;i++) {
		for(j=0;j<256;j++) {
			if(tau[j] == i)
				tau_inv[i] = j;
		}
	}
	// TODO REMOVE OUTPUT TO FILE
	FILE *conjs = fopen("conjs.txt", "w");
	// calculate sigma_conj's
	for(i=0;i<n;i++) {
		// CALCULATE TAU_TTI
		// start with identity
		for(j=0;j<256;j++)
			tau_tti[j] = j;
		// multiply by tau i times
		for(j=0;j<i;j++) {
			for(k=0;k<256;k++)
				tmp[k] = tau[tau_tti[k]];
			for(k=0;k<256;k++)
				tau_tti[k] = tmp[k];
		}
		// CALCULATE TAU_INV_TTI
		for(j=0;j<256;j++)
			tau_inv_tti[j] = j;
		for(j=0;j<i;j++) {
			for(k=0;k<256;k++)
				tmp[k] = tau_inv[tau_inv_tti[k]];
			for(k=0;k<256;k++)
				tau_inv_tti[k] = tmp[k];
		}
		// CALCULATE SIGMA_CONJ+I*256
		for(j=0;j<256;j++)
			*(sigma_conj+i*256+j) = tau_tti[*(sigma+i*256+tau_inv_tti[j])];
		fprintf(conjs, "sigma hat %d\n", i);
		for(j=0;j<256;j++) {
			fprintf(conjs, "%02x-%02x ", j, *(sigma_conj+i*256+j));
			if((j+1)%8==0)
				fprintf(conjs, "\n");
		}
	}


	// INIT SIGMA
	int real_sigma[256] = {0},
	    instances[256] = {0}, // instances of a particular wire endpoint being seen in a sigma_conj
	    max, times_seen;
	// FOR EACH WIRE
	for(i=0;i<256;i++) {
	// 	FIND MAX CONJUGATE
	// 	for each conjugate
		//memset(instances, 0, 256*sizeof(int));
		for(j=0;j<256;j++)
			instances[j] = 0;
		for(j=0;j<n;j++)
			instances[*(sigma_conj+j*256+i)]++;
		max = -1;
		times_seen = 0;
		for(j=0;j<256;j++) {
			if(instances[j]>times_seen) {
				max = j;
				times_seen = instances[j];
			}
		}
		fprintf(stderr, "times_seen: %d\n", times_seen);
		real_sigma[i] = max;
	}
	// OUTPUT SIGMA

	for(i=0;i<256;i++) {
		printf("%02x-%02x ", i, real_sigma[i]);
		if((i+1) % 8 == 0)
			printf("\n");
	}
	// CLEANUP
	free(sigma);
	free(sigma_conj);
}
