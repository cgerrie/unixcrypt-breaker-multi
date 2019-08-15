#include <stdio.h>

int main(int argc, char *argv[]) {
	int i;
	int sigma[256], sigma_prime[256], tau[256];

	for(i=0;i<256;i++)
		sigma[(256-i)%256] = fgetc(stdin)+i;
	for(i=0;i<256;i++)
		sigma_prime[(256-i)%256] = fgetc(stdin)+i;
	//for(i=0;i<256;i++)
	//	tau[i] = sigma[i]

	return 0;

}
