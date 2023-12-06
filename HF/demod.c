#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	complex float in;
	uint8_t prev_bit=0, bit=0, out=0;
	while (1)
	{
		int k = fread(&in, sizeof(complex float), 1, stdin);
		if (feof(stdin))
		{
			break;
		}
		if (k > 0)
		{
			if(crealf(in) > 0.0f){
				bit = 1;
			}else{
				bit = 0;
			}

			out = (bit ^ prev_bit) & 1;

			prev_bit = bit;
			fwrite(&out, sizeof(uint8_t), 1, stdout);
		}
		else
		{
			usleep(1000);
		}
	}

	return 0;
}
