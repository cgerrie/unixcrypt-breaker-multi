#include <stdio.h>

int main(int argc, char *argv[]) {
	if(argc<2) {
		fprintf(stderr, "not enough arguments\n");
		return 1;
	}
	FILE *out = fopen(argv[1], "w");
	printf("%s\n", argv[1]);
	int i, c;
	for(i=0;i<2;i++)
		for(c=0;c<256;c++)
			fprintf(out, "%c", 0);
	fclose(out);
	return 0;
}

