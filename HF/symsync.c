#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

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
		assert(sps % 2 == 0);
	}
	
	// 0 a legujabb, 1 az elozo, 2 a kettovel ezelotti
	complex float out,in[3];
	float max = 0.f;
	size_t counter = 0;
	float timing_error = 0.f;
	const float TIMING_INC = 0.01f;
	//samples[index] and samples[(index+sps/2)%sps] will be sent
	//one of which is maximal, the other is minimal(0)
	size_t index = 0;
	uint8_t prevbit = 0;
	while(1)
	{
		int k = fread(&in,sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			if(++counter % 2){
			//interpolate between the three points
			/* fprintf(stderr,"%+1.2f %+1.2f %+1.2f\t->\t",crealf(in[2]),crealf(in[1]),crealf(in[0])); */
			in[0] = in[0] * sinc(timing_error)		+ in[1] * sinc(1.f+timing_error)	+ in[2] * sinc(2+timing_error);
			in[1] = in[0] * sinc(-1.f+timing_error) + in[1] * sinc(timing_error)		+ in[2] * sinc(1+timing_error);
			in[2] = in[0] * sinc(-2.f+timing_error) + in[1] * sinc(-1.f+timing_error)	+ in[2] * sinc(timing_error);
			/* fprintf(stderr,"%+1.2f %+1.2f %+1.2f ",crealf(in[2]),crealf(in[1]),crealf(in[0])); */
			//
			//Detect early-late
			float sign = crealf(in[1] * (in[0]-in[2]));
			bool early = sign > 0;
			/* fprintf(stderr, "%s: t_error:%+2.3f\n", early ? "early" : "late ",timing_error); */
			timing_error += early ? TIMING_INC : -TIMING_INC;
			if(timing_error > 1.f){
				timing_error = -1.f;
			}
			if(timing_error < -1.f){
				timing_error = 1.f;
			}
			out = in[1];
			fwrite(&out,sizeof(complex float),1,stdout);
			}
			//shift everything
			in[2] = in[1];
			in[1] = in[0];
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
