#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	int i;
	// input wires directory?
	// init tau
	int tau[256] = {0};
	// for i=0 to 255
	FILE *wire_file = NULL;
	char file_name[100];
	for(i=0;i<256;i++) {
	// 	access wires/i.wire
		sprintf(&file_name, "%s/%d.wire", argv[1], i);
		wire_file = fopen(file_name, "r");
	// 	set in tau
		fscanf(wire_file, "%d", &tau[i]);
	}
	// output tau & to to file
	sprintf(&file_name, "%s/tau", argv[1]);
	FILE *tau_file = fopen(file_name, "w");
	for(i=0;i<256;i++) {
		fprintf(tau_file, "%02x-%02x ", i, tau[i]);
		printf("%02x-%02x ", i, tau[i]);
		if((i+1) % 8 == 0) {
			fprintf(tau_file, "\n");
			printf("\n");
		}
	}
}
