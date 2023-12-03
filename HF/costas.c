#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	long fs = 200e3;
	if(argc > 1)
	{
		fprintf(stderr,"%s set fs=%s\n",argv[0],argv[1]);
		fs = atol(argv[1]);
	}
	//ide tarolom el a szinusz jelemet
	complex float* m = malloc(sizeof(complex float)*fs);
	if(m==NULL)
	{
		fprintf(stderr,"memory allocation failure! %s\n",argv[0]);
		return -1;
	}
	
	//feltoltom a memoriat a szinusz kepevel
	for(long i = 0; i<fs; i++){
		m[i]=cos(2*M_PI*i/fs);
	}
	complex float out=0+0*I,in=0+0*I;
	size_t index = 0;
	float conversion = fs/(2*M_PI);
	const float Ko=1,Ki=0.01,Kp=0.5;
	float Pe=0,Pin=0,Pintegrator_prev=0,Phat=0, Pf=0;
	while(1)
	{
		int k = fread(&in,sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			//PD
			Pin = carg(in);
			Pe = Pin - Phat;
			//LOOP FILTER
			Pf = (Kp * Pe) + (Ki * Pe + Pintegrator_prev); // proportional + integrator
			Pintegrator_prev = Ki * Pe + Pintegrator_prev;
			//NCO
			out = m[index];
			index = (index + (size_t)(Pf * Ko * conversion)) % fs;

			fwrite(&out,sizeof(complex float),1,stdout);
			Phat = carg(out);
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
