#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>

//source: https://wirelesspi.com/costas-loop-for-carrier-phase-synchronization/
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
		m[i]=cexp(2*M_PI*i/fs);
	}
	complex float out=0+0*I,in=0+0*I;
	size_t index = 0;
	float conversion = fs/(2*M_PI);
	const float Ko=5,Ki=0.01,Kp=0.5;
	float Pe=0,Pin=0,Pintegrator_prev=0,Phat=0, Pf=0;
	complex float y=0+0*I;
	float lpf1=0,lpf2=0;
	float loop_filter_input=0,loop_filter_output=0;
	const float LPF_COEF = 0.4;
	//----

	while(1)
	{
		int k = fread(&in,sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			//SECOND MULTIPLIER
			loop_filter_input = lpf1 * lpf2;
			//LOOP FILTERING
			loop_filter_output = (Kp * Pe) + (Ki * Pe + Pintegrator_prev); // proportional + integrator
			Pintegrator_prev = Ki * Pe + Pintegrator_prev;
			//VCO
			complex float vco = m[index];
			index = (index + (size_t)(loop_filter_output * conversion)) % fs;
			float vco_real = crealf(vco);
			float vco_imag = cimagf(vco);
			//FIRST MULTIPLIER
			float xi = crealf(in * vco_real);
			float xq = crealf(in * vco_imag);

			//LPF IIR
			/* y += 0.01 * (x - y) */
			lpf1 += LPF_COEF * (xi - lpf1);
			lpf2 += LPF_COEF * (xq - lpf2);

			out = lpf1+I*lpf2;

			fwrite(&out,sizeof(complex float),1,stdout);
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
