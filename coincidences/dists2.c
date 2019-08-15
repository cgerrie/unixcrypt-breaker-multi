#include <stdio.h>

#define N 65536 // TODO TEMPORARY
#define RUNS 10000000
#define MAX_K 69
#define MAX_COINC 20

unsigned int *randomdata;
FILE *urandom;

static inline int coincidences(const int k) {
	static int i, j, coincs;
	// generate random data
	fread(randomdata, sizeof(int), k, urandom);
	for(i=0;i<k;i++)
		randomdata[i] = randomdata[i] % N; // SINCE N=256=0xFF
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
	sample = 69;
	//for(sample=1;sample<=MAX_K;sample++) {
		fprintf(stderr,"sampling k=%d\n",sample);
		for(run=0;run<RUNS;run++) {
			samples[sample-1][coincidences(sample)]++;
			//fprintf(stderr, "incing %d\n", sample-1);
		}
		/*for(run=0;run<MAX_COINC;run++) {
			samples_d[sample][run] = (double)samples[sample][run];
			samples_d[sample][run] /= RUNS;
		}*/
	//}
	// output data
	printf("sample\\coincs");
	for(sample=0;sample<MAX_COINC;sample++)
		printf("\t%d",sample);
	printf("\n");
	sample = 69;
	//for(sample=1;sample<=MAX_K;sample++) {
		printf("%d",sample);
		for(run=0;run<MAX_COINC;run++)
			printf("\t%d",samples[sample-1][run]);
		printf("\n");
	//}
	fprintf(stderr,"finished outputing data\n");
	// end
	fclose(urandom);
	free(randomdata);
	return 0;
}
