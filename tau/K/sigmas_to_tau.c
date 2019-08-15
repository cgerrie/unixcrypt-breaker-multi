#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#include "list.h"

#define TABLE_WIDTH 20

struct wire_s {
	unsigned int from, to;
};
typedef struct wire_s wire;

struct wire_count_s {
	wire w;
	int count;
};
typedef struct wire_count_s wire_count;

// wire comparison function for sorting
int comp_wires(const wire *x, const wire *y) {
	if(x == NULL || y == NULL)
		return 1; // if null, arbitrary
	if(x->from<y->from)
		return -1;
	if(x->from>y->from)
		return 1;
	if(x->to<y->to)
		return -1;
	if(x->to>y->to)
		return 1;
	return 0;
}


// returns list (sorted?) of other wires that are consequences of the sigmas
void conseq(int *sigma, int n, wire w, wire *consequences) {
	int i;
	for(i=0;i<n-1;i++) {
		consequences[i] = (wire){*(sigma+(i+1)*256+w.from), *(sigma+i*256+w.to)};
	}
	qsort(consequences, n-1, sizeof(wire), comp_wires);
}

// returns the number of shared consequences of the two wires
int coincedences(wire *w_x, wire *w_y, int n, wire *consequences) {
	int acc = 0, i, j, cmp, n_x, n_y;
	wire *consequences_x = consequences + (256*w_x->from + w_x->to)*(n-1),
	     *consequences_y = consequences + (256*w_y->from + w_y->to)*(n-1),
	     curr;
	for(i=0, j=0;i<n-1 && j<n-1;) { // two-finger walking algorithm
		cmp = comp_wires(consequences_x+i, consequences_y+j);
		if(cmp==-1)
			i++;
		else if(cmp==1)
			j++;
		else { // if cmp = 0
			// count number in x
			for(n_x = 0, curr = *(consequences_x+i);
			    comp_wires(consequences_x+i,&curr)==0;
			    i++, n_x++);
			// count number in y
			for(n_y = 0, curr = *(consequences_y+j);
			    comp_wires(consequences_y+j,&curr)==0;
			    j++, n_y++);
			// add to accumulator
			acc += n_x * n_y;
		}
	}
	return acc; // PLACEHOLDER
}

int main(int argc, char *argv[]) {
	int i, j, k;
	int from_char;
	// INPUT
	//int from_char = 2; // default
	/*if(argc > 1) {
		from_char = strtol(argv[1], NULL, 10);
		
	}*/
	if(argc < 2) {
		fprintf(stderr, "output directory not specified\n");
		return 1;
	}
	fprintf(stderr, "considering wires going from %d\n", from_char);

	// READ SIGMAS
	int n = 0,
	    *sigma = NULL;
	sigma = (int*)malloc(1);
	bool not_done = true;
	// for each sigma
	while(not_done) {
		//fprintf(stderr, "reallocing n=%d\n", n+1);
		sigma = realloc(sigma, (++n)*256*sizeof(int));
		
		// skip headeri
		fscanf(stdin, "wires %*d\n");
		// read all wires
		for(i=0;i<256;i++) {
			if(fscanf(stdin, "%*x-%x ", sigma+((n-1)*256+i))!=1) {
				n--;
				not_done = false;
				break;
			}
		}
		// skip ender
		fscanf(stdin, "end wires\n");
	}
	
	/*for(i=0;i<n;i++) {
		fprintf(stderr, "sigma %d\n", i);
		for(j=0;j<256;j++)
			fprintf(stderr, "%02x-%02x ", j, *(sigma+(i*256+j)));
		fprintf(stderr, "\n");
	}*/
	
	// CALCULATE CONSEQ FOR ALL WIRES
	// store in array (n sigmas -> n-1 consequences each)
	fprintf(stderr, "calculating consequences\n");
	wire *consequences = NULL;
	consequences = malloc(256*256*(n-1)*sizeof(wire));
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			conseq(sigma, n,
			       (wire){i,j},
			       consequences+(i*256+j)*(n-1));

	// CALCULATE COINCEDENCES FOR ALL PAIRS OF WIRES
	// AND TABULATE
	/*fprintf(stderr, "calculating coincidences\n");
	int K[256][TABLE_WIDTH] = {0}, // TODO DECLARE DYNAMICALLY SO THAT IT CAN BE VARIABLE
	    coinc;
	wire w1, w2;
	w1.from = from_char;
	for(i=0;i<256;i++) {
		if(i % 16 == 0)
			fprintf(stderr, "%d/256 done\n",i);
		w1.to = i;
		for(j=0;j<256;j++) {
			w2.from = j;
			for(k=0;k<256;k++) {
				if(from_char != j && i != k && from_char != i && j != k) { // TODO BREAK UP THIS CHECK
				w2.to = k;
					coinc = coincedences(&w1, &w2, n, consequences);
					if(coinc >= TABLE_WIDTH) { // TODO DYNAMIC SO THIS ISN'T FATAL
						fprintf(stderr, "table not wide enough\n");
						return 1;
					}
					K[i][coinc]++;
				}
			}
		}

	}*/
	// count how many wires have each wire as a consequence
	int coincedences_length[256][256] = {0},
	    coincedences_proc_length[256][256] = {0};
	wire *coincedences[256][256],
	     *coincedences_tmp[256][256];
	     w;
	wire_count *coincedences_proc[256][256];
	// coincidences = preimage multisets of consequences
	for(i=0;i<256;++) {
		for(j=0;j<256;j++) {
			for(k=0;k<n-1;k++) {
				w = *(consequences+(n-1)*(256*i+j)+k);
				coincedences_length[w.from][w.to]++;
			}
		}
	}
	for(i=0;i<256;i++) {
		for(j=0;j<256;j++) {
			coincedences[i][j] = malloc(coincedences_length[i][j]*sizeof(wire));
			coincedences_tmp[i][j] = coincidences[i][j];
		}
	}
	for(i=0;i<256;i++) {
		for(j=0;j<256;j++) {
			for(k=0;k<n-1;k++) {
				w = *(consequences+(n-1)*(256*i+j)+k);
				*(coincedences_tmp[w.from][w.to]) = w;
				coincedences_tmp[w.from][w.to]++;
			}
		}
	}
	// TODO preprocess coincedences into sorted struct form
	// SORT
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			qsort(coincidences[i][j], coincedences_length[i][j], sizeof(wire), comp_wires);
	// CONVERT TO STRUCTS
	// count
	wire curr = NULL;
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			for(k=0;k<coincedences_length;k++)
				if(comp_wires(curr, curr = *(coincedences[i][j]+k)) != 0) // TODO IS THIS A MISUSE OF ARGUMENTS?!
					coincedences_proc_length[i][j]++;
	// init
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			coincendences_proc[i][j] = malloc(coincedences_proc_length[i][j]*sizeof(wire_count));
	// fill
	// TODO



	// TODO FREE COINCEDENCES AS SOON AS POSSIBLE

	// count coincedences
	int K[256][TABLE_WIDTH],
	    J[256][256][256];
	FILE *out_file;
	char filename[80];
	for(from_wire=0;from_wire<256;from_wire++) {
		K = {0};
		// process coincidences
		for(i=0;i<256;i++) {
			for(j=0;j<256;j++) {
				// TODO IDEA preprocess coincedences by replacing by multiset tags with amounts?
				// add all intersections in coincidences[i][j] to the correct K
				for(k=0;k<coincedences_length[i][j];k++) { // TODO CHANGE THIS TO PREPROCESSED COINCEDENCES
					// if correct from_wire's, process
					// ADD product of instances to correct J
				}
			}
		}
		// sum J rows to increment correct K
		// padd K[_][0]'s
		// TODO OUTPUT K TO from_wire'th SPECIFIC FILE
		filename = {0};
		// copy argv[1] into filename, then append "/<from_wire>.wire"
		out_file = fopen(filename, "w");

	}
	// update K
	// free coincidencces, coincidences_tmp
	// TODO DID YOU CHECK FOR 64771 BOUNDARY CONDITIONS?
	// TODO PADDED OUT 0 COINCIDENCES CASES??




	// OUTPUT K
	// TODO REMOVE/COMMENT OUT
	printf("data for wire from %d\n", from_char);
	printf("sampled from %d sigmas\n", n);
	printf("to \\ coincidences");
	for(i=0;i<TABLE_WIDTH;i++)
		printf("\t%d", i);
	printf("\n");
	for(i=0;i<256;i++) {
		printf("%d", i);
		for(j=0;j<TABLE_WIDTH;j++)
			printf("\t%d", K[i][j]);
		printf("\n");
	}
	printf("\n");


	// CLEAN UP
	free(sigma);
	free(consequences);
	fprintf(stderr, "done\n");
	return 0;
}
