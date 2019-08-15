#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	int i, j;
	char c;
	// INPUT DATA FILE (OUTPUT OF SIGMAS_TO_TAU
	fprintf(stderr, "inputing header\n");
	int wire_start = -1, number_of_sigmas = -1, table_width = -1;
	fscanf(stdin, "%*s %*s %*s %*s %d", &wire_start); // "data for wire from %d"
	fscanf(stdin, "%*s %*s %d %*s", &number_of_sigmas); // "sampled from %d sigmas"
	fscanf(stdin, "%*s %*s %*s"); // "to \ coincidences"
	while(fscanf(stdin, "%d%c ", &table_width, &c)==2 && c!='\n');
	table_width++; // while loop counts highest label, but 0 must also be included
	fprintf(stderr, "done reading header. table_width=%d\n", table_width);

	// TODO CAN I STORE THESE AT THIS POINT AS DOUBLES?
	//unsigned int *data;
	double *data;
	data = malloc(256*table_width*sizeof(double));
	fprintf(stderr, "reading data\n");
	unsigned int curr;
	for(i=0;i<256;i++) {
		scanf("%*d"); // skip which wire being read
		for(j=0;j<table_width;j++) {
			scanf("%u", &curr);
			*(data+i*20+j) = (double)curr;
		}
	}
	fprintf(stderr, "done reading data\n");
	// THROW OUT 0 & 1'S COLUMN!
	// RENORMALIZE REST TO SUM TO ~100K?
	double sum;
	for(i=0;i<256;i++) {
		sum = 64771 - *(data+i*20) - *(data+i*20+1);
		*(data+i*20) = 0;
		*(data+i*20+1) = 0;
		for(j=2;j<table_width;j++)
			*(data+20*i+j) = 100000*(*(data+20*i+j))/sum;
	}
	fprintf(stderr, "done processing why won't you go here!\n");
	// OUTPUT
	printf("data for wire from %d\nsampled from %d sigmas\nto \\ coincidences", wire_start, number_of_sigmas);
	for(i=0;i<table_width;i++)
		printf("\t%d", i);
	printf("\n");
	for(i=0;i<256;i++) {
		printf("%d\t", i);
		for(j=0;j<table_width;j++)
			printf("%d\t", (int)*(data+i*20+j));
		printf("\n");
	}
	// CLEANUP
	free(data);

	return 0;
}
