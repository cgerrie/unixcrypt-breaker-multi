#include<stdio.h>

//#define USE_NON_HEX 1


int main(int argc, char *argv[]) {
	
	
	int i, j, rho[256], pi[256], rho_inv[256], tau[256], sigma[256], tmp[256];
	// read rho
	scanf("%*s %*s");
	for(i=0;i<256;i++)
		fscanf(stdin, "%*x%*c%x", &rho[i]);//!=1)
			//return 1;
	// read pi
	scanf("%*s %*s");
	for(i=0;i<256;i++)
		fscanf(stdin, "%*x%*c%x", &pi[i]);
	// read rho_inv
	scanf("%*s %*s");
	for(i=0;i<256;i++)
		fscanf(stdin, "%*x%*c%x", &rho_inv[i]);
	// calculate tau from tau = rho_inv kappa rho
	memcpy(tau, rho, 256*sizeof(int));
	for(i=0;i<256;i++)
		tmp[i] = (tau[i]+1)%256;
	for(i=0;i<256;i++)
		tau[i] = rho_inv[tmp[i]];
	// calculate sigma from sigma = rho_inv pi rho
	memcpy(sigma, rho, 256*sizeof(int));
	for(i=0;i<256;i++)
		tmp[i] = pi[sigma[i]];
	for(i=0;i<256;i++)
		sigma[i] = rho_inv[tmp[i]];
	// output tau, sigma, tau_inv
	FILE *stream = stdout;
	fprintf(stream, "tau\n");
	j=0;
	for(i=0;i<256;i++) {
#ifdef USE_NON_HEX
		fprintf(stream,"%03d-%03d ",i,tau[i]);
#endif
#ifndef USE_NON_HEX
		fprintf(stream,"%02x-%02x ",i,tau[i]);
#endif
		j++;
		if(j==8) {
			fprintf(stream,"\n");
			j=0;
		}
	}
	fprintf(stream, "sigma\n");
	j=0;
	for(i=0;i<256;i++) {
#ifdef USE_NON_HEX
		fprintf(stream,"%03d-%03d ",i,sigma[i]);
#endif
#ifndef USE_NON_HEX
		fprintf(stream,"%02x-%02x ",i,sigma[i]);
#endif
		j++;
		if(j==8) {
			fprintf(stream,"\n");
			j=0;
		}
	}
	// or n sigma_i's
	return 0;
}
