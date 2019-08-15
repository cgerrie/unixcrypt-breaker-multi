#include <stdio.h>
#include <stdlib.h>

int main(void) {
	int wire = 0;
	fscanf(stdin, "%*s %*s %*s %*s %d\n", &wire);
	printf("%d\n",wire);
	printf("%d\n",fscanf(stdin, "%*s %*s %*s "));
	int n = 0;
	char *derp = malloc(20);
	//while(fscanf(stdin, " %s ",derp)>=0);
	do
		printf("n:%d c:%c\n",n,derp[0]);
	while(fscanf(stdin, "%d%c", &n,derp)==2 && derp[0]!='\n');
	printf("last n:%d c:%c\n",n,derp[0]);
}
