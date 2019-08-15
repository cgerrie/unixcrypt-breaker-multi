#include <stdio.h>

#define N 65536 // TODO TEMPORARY
#define RUNS 1000000
#define MAX_K 50
#define MAX_COINC 20

int *randomdata;
FILE *urandom;

static inline int coincidences(const int k) {
	static int i, j, coincs;
	// generate random data
	fread(randomdata, sizeof(int), k, urandom);
	for(i=0;i<k;i++)
		randomdata[i] = randomdata[i] & 0xffff; // SINCE N=256=0xFF
	// find coincedences
	coincs = 0;
	for(i=0;i<k;i++)
		for(j=i+1;j<k;j++)
			if(randomdata[i]==randomdata[j])
				coincs++;
	// return
	//if(coincs!=0)
	//	fprintf(stderr,"not zero\n");
	return coincs;
}

int main(int argc, char *argv[]) {
	urandom = fopen("/dev/urandom","r");
	randomdata = malloc(MAX_K*sizeof(int));

	int run, sample, samples[MAX_K][MAX_COINC] = {0};
	double samples_d[MAX_K][MAX_COINC] = {0};
	for(sample=1;sample<=MAX_K;sample++) {
		fprintf(stderr,"sampling k=%d\n",sample);
		for(run=0;run<RUNS;run++)
			samples[sample-1][coincidences(sample)]++;
		/*for(run=0;run<MAX_COINC;run++) {
			samples_d[sample][run] = (double)samples[sample][run];
			samples_d[sample][run] /= RUNS;
		}*/
	}
	// output data
	printf("sample\\coincs");
	for(sample=1;sample<=MAX_COINC;sample++)
		printf("\t%d",sample);
	printf("\n");
	for(sample=0;sample<MAX_K;sample++) {
		printf("%d",sample);
		for(run=0;run<MAX_COINC;run++)
			printf("\t%d",samples[sample][run]);
		printf("\n");
	}
	fprintf(stderr,"finished outputing data\n");
	// end
	fclose(urandom);
	free(randomdata);
	return 0;
}
