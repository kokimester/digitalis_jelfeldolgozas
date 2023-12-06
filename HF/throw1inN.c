#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>


int main(int argc, char** argv)
{
	long N = 2;
	if(argc > 1)
	{
		fprintf(stderr,"%s set N=%s\n",argv[0],argv[1]);
		N = atol(argv[1]);
	}
	
	uint8_t in,out;
	size_t counter = 0;
	while(1)
	{
		int k = fread(&in,sizeof(uint8_t),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
				counter++;
				if(counter >= N){
					out = in;	
					fwrite(&out, sizeof(uint8_t), 1, stdout);
					counter = 0;
				}
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
