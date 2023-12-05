#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	long n = 1;
	if (argc > 1)
	{
		n = atol(argv[1]);
		n = n > 0 ? n : -n;
	}
	complex float in, out;
	long count = 0;
	while (1)
	{
		int k = fread(&in, sizeof(complex float), 1, stdin);
		if (feof(stdin))
		{
			break;
		}
		if (k > 0)
		{
				count++;
				if (count >= n)
				{
					count = 0;
					out = in;
					fwrite(&out, sizeof(complex float), 1, stdout);
				}
		}
		else
		{
			usleep(1000);
		}
	}

	return 0;
}
