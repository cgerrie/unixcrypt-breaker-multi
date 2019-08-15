#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	int i;
	// INPUT TAU
	int tau[256] = {0};
	for(i=0;i<256;i++)
		scanf("%*x%*c%x",&tau[i]);
	// INPUT SIGMAS
	int n = 0,
	    *sigma = NULL;
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
	// TODO REMOVE TEST
	for(i=0;i<n;i++) {
		printf("sigma %d 00-%x\n", i, *(sigma+256*i));
	}

	// CALCULATE TAU CONJUGATES
	int tau_inv[256] = {0},
	    tau_tti[256] = {0},
	    tau_inv_tti[256] = {0},
	    tmp[256] = {0},
	    *sigma_conj = NULL;
	sigma_conj = malloc(n*256*sizeof(int));
	
	// OUTPUT
	// CLEANUP

	return 0;
}
