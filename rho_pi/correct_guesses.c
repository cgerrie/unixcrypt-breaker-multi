#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	if(argc<3) {
		fprintf(stderr, "not enough arguments");
		return 1;
	}
	FILE *first = fopen(argv[1], "r"),
	     *second = fopen(argv[2], "r");
	int first_perm[256] = {0},
	    second_perm[256] = {0},
	    x = 0, y=0;
	// read from first to get first_perm
	while(fscanf(first, "%x-%x", &x, &y)>0)
		first_perm[x] = y;
	// read from second to get second_perm
	while(fscanf(second, "%x-%x", &x, &y)>0)
		second_perm[x] = y;
	// compare how many correct guesses
	y=0;
	for(x=0;x<256;x++)
		if(first_perm[x] == second_perm[x])
			y++;
	printf("%d correct\n",y);

	// end
	fclose(first);
	fclose(second);
	return 0;
}
