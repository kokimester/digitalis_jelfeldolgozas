#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>

float sinc(float x){
	if(fabsf(x)<0.001f) return 1;
	return sinf(x*M_PI)/(x*M_PI);
}



int main(int argc, char** argv)
{
	long sps = 2;
	if(argc > 1)
	{
		fprintf(stderr,"%s set sps=%s\n",argv[0],argv[1]);
		sps = atol(argv[1]);
	}
	
	// 0 a legujabb, 1 az elozo, 2 a kettovel ezelotti
	complex float in[sps],out;
	for(size_t i = 0; i<sps; ++i){
		in[i]=0+0*I;
	}
	float timing_error = 0;
	const float TIMING_INC = 0.1;
	size_t counter = 0;
	while(1)
	{
		int k = fread(&in[0],sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			if(++counter % 2){
			//interpolate between prev_in and in
			fprintf(stderr,"%+1.2f %+1.2f %+1.2f\t->\t",crealf(in[2]),crealf(in[1]),crealf(in[0]));
			in[0] = in[0] * sinc(timing_error)		+ in[1] * sinc(1.f+timing_error)	+ in[2] * sinc(2+timing_error);
			in[1] = in[0] * sinc(-1.f+timing_error) + in[1] * sinc(timing_error)		+ in[2] * sinc(1+timing_error);
			in[2] = in[0] * sinc(-2.f+timing_error) + in[1] * sinc(-1.f+timing_error)	+ in[2] * sinc(timing_error);
			fprintf(stderr,"%+1.2f %+1.2f %+1.2f ",crealf(in[2]),crealf(in[1]),crealf(in[0]));
			//
			//Detect early-late
			float sign = crealf(in[1] * (in[0]-in[2]));
			bool early = sign > 0;
			fprintf(stderr, "%s: t_error:%+2.3f\n", early ? "early" : "late ",timing_error);
			timing_error += early ? TIMING_INC : -TIMING_INC;
			if(timing_error > 1.f){
				timing_error = -1.f;
			}
			/* fprintf(stderr, "%1.2f %1.2f %1.2f\n", sinc(1+timing_error), sinc(timing_error), sinc(-1.f+timing_error)); */
			}
			//shift everything
			out = in[1];
			in[2] = in[1];
			in[1] = in[0];
			fwrite(&out,sizeof(complex float),1,stdout);
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
