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
		m[i]=cexpf(2*M_PI*i/fs);
	}
	complex float out=0+0*I,in=0+0*I;
	size_t index = 0;
	float conversion = fs/(2*M_PI);
	const float Ko=1,Ki=0.001,Kp=0.5;
	float Pe=0,Pin=0,Pintegrator_prev=0,Phat=0, Pf=0;
	//----
	//
	/* float        phase_offset     = 0.00f;  // carrier phase offset */
    /* float        frequency_offset = 0.30f;  // carrier frequency offset */
    /* float        wn               = 0.01f;  // pll bandwidth */
    /* float        zeta             = 0.707f; // pll damping factor */
    /* float        K                = 1000;   // pll loop gain */

    /* // generate loop filter parameters (active PI design) */
    /* float t1 = K/(wn*wn);   // tau_1 */
    /* float t2 = 2*zeta/wn;   // tau_2 */

    /* // feed-forward coefficients (numerator) */
    /* float b0 = (4*K/t1)*(1.+t2/2.0f); */
    /* float b1 = (8*K/t1); */
    /* float b2 = (4*K/t1)*(1.-t2/2.0f); */

    /* // feed-back coefficients (denominator) */
    /* //    a0 =  1.0  is implied */
    /* float a1 = -2.0f; */
    /* float a2 =  1.0f; */

    /* // filter buffer */
    /* float v0=0.0f, v1=0.0f, v2=0.0f; */

    /* // initialize states */
    /* float phi     = phase_offset;   // input signal's initial phase */
    /* float phi_hat = 0.0f;           // PLL's initial phase */

	while(1)
	{
		int k = fread(&in,sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
		/* // compute input sinusoid and update phase */

        /* // compute PLL output from phase estimate */
        /* complex float y = cosf(phi_hat) + I*sinf(phi_hat); */

        /* // compute error estimate */
        /* float delta_phi = cargf( in * conjf(y) ); */

        /* // push result through loop filter, updating phase estimate */
        /* v2 = v1;  // shift center register to upper register */
        /* v1 = v0;  // shift lower register to center register */
        /* v0 = delta_phi - v1*a1 - v2*a2; // compute new lower register */

        /* // compute new output */
        /* phi_hat = v0*b0 + v1*b1 + v2*b2; */
			//
			//
			//
			//
			//

			

			//SECOND MULTIPLIER
			loop_filter_input = lpf1 * lpf2;
			//LOOP FILTERING
			loop_filter_output = (Kp * Pe) + (Ki * Pe + Pintegrator_prev); // proportional + integrator
			Pintegrator_prev = Ki * Pe + Pintegrator_prev;
			//VCO
			complex float vco = m[index];
			index = (index + (size_t)(loop_filer_output * conversion)) % fs;
			float vco_real = crealf(vco);
			float vco_imag = cimagf(vco);
			//FIRST MULTIPLIER
			float xi = in * vco_real;
			float xq = in * vco_imag;

			//LPF IIR
			/* y += 0.01 * (x - y) */
			lpf1 += 0.01 (xi - lpf1);
			lpf2 += 0.01 (xq - lpf2);

			out = lpf1;

			//PD
			Pin = cargf(in);
			Pe = Pin - Phat;
			//LOOP FILTER
			Pf = (Kp * Pe) + (Ki * Pe + Pintegrator_prev); // proportional + integrator
			Pintegrator_prev = Ki * Pe + Pintegrator_prev;
			//NCO
			out = m[index];
			index = (index + (size_t)(Pf * Ko * conversion)) % fs;

			Phat = carg(out);
		out = y;
			fwrite(&out,sizeof(complex float),1,stdout);
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
