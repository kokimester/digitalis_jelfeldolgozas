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
	complex float samples[sps],out,in;
	for(size_t i = 0; i<sps; ++i){
		samples[i]=0+0*I;
	}
	float max = 0.f;
	size_t counter = 0;
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
			if(counter++ < sps){
				for(size_t i = sps-1; i > 0; --i){
					samples[i] = samples[i-1];
				}
				samples[0] = in;
			}
			else{
				counter = 0;
				max = 0.f;
				for(size_t i = 0; i<sps;++i){
					float current_value = fabsf(crealf(samples[i]));
					/* fprintf(stderr, "%1.2f\t",crealf(samples[i])); */
					if(current_value > max){
						index = i;
						max = current_value;
					}
				}
				/* fprintf(stderr, "\n"); */
				/* fwrite(&samples[(index+(sps/2))%sps],sizeof(complex float),1,stdout); */
				/* fwrite(&samples[index],sizeof(complex float),1,stdout); */
				uint8_t bit = crealf(samples[index]) > 0.f ? 1 : 0;
				unsigned char output = (bit ^ prevbit) & 1;
				prevbit=bit;
				/* fprintf(stderr, "decoded: %u\n",bit); */
				fwrite(&output, sizeof(uint8_t), 1, stdout);
			}
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
