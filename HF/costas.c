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
		m[i]=cexp(I*2*M_PI*i/fs);
		/* fprintf(stderr, "%+1.2f%+1.2fi\n",crealf(m[i]),cimagf(m[i])); */
	}
	complex float out=0+0*I,in=0+0*I;
	size_t index = 0;
	float conversion = fs/(2*M_PI);
	const float Ko=5,Ki=0.01,Kp=0.5;
	float Pe=0,Pin=0,Pintegrator_prev=0,Phat=0, Pf=0;
	complex float y=0+0*I;
	float lpf1=1.f,lpf2=1.f;
	float loop_filter_input=0,loop_filter_output=0;
	const float LPF_COEF = 0.01;
	//----
	//
	//GNURADIO
	float d_phase=0.f;
	float d_freq=0.f;
	float d_error=0.f;

	while(1)
	{
		int k = fread(&in,sizeof(complex float),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			//GNURADIO
			const float d_damping = 1.f;
			const float d_loop_bw = 0.06f;
			float denom = (1.0 + 2.0 * d_damping * d_loop_bw + d_loop_bw * d_loop_bw);
			float d_alpha = (4 * d_damping * d_loop_bw) / denom;
			float d_beta = (4 * d_loop_bw * d_loop_bw) / denom;
			//vco
			complex float vco = cosf(-d_phase) + I*sinf(-d_phase);
			//mix
			out = in * vco;
			d_error = crealf(out) * cimagf(out);
			//clip d_error to +-1.0f
			float x1 = fabsf(d_error + 1.f);
			float x2 = fabsf(d_error - 1.f);
			x1 -= x2;
			d_error = 0.5 * x1;
			//advance
			d_freq = d_freq + d_beta * d_error;
			d_phase = d_phase + d_freq + d_alpha * d_error;

			while (d_phase > (2 * M_PI))
				d_phase -= 2 * M_PI;
			while (d_phase < (-2 * M_PI))
				d_phase += 2 * M_PI;
			//GNURADIO
			//
			/* //VCO */
			/* complex float vco = m[index]; */
			/* index = (index + (size_t)(loop_filter_output * conversion)) % fs; */
			/* float vco_real = crealf(vco); */
			/* float vco_imag = cimagf(vco); */
			/* /1* fprintf(stderr, "[%u]%+1.2f%+1.2f | %+1.2f -> %+1.2f\n",index,vco_real,vco_imag,loop_filter_output, loop_filter_output*conversion); *1/ */
			/* //FIRST MULTIPLIER */
			/* float xi = crealf(in * vco_real); */
			/* float xq = crealf(in * vco_imag); */

			/* //LPF IIR */
			/* /1* y += 0.01 * (x - y) *1/ */
			/* lpf1 += LPF_COEF * (xi - lpf1); */
			/* lpf2 += LPF_COEF * (xq - lpf2); */
			
			/* //SECOND MULTIPLIER */
			/* loop_filter_input = lpf1 * lpf2; */
			/* //LOOP FILTERING */
			/* Pe = loop_filter_input; */
			/* /1* fprintf(stderr,"Kp * Pe + Ki*Pe + Pintegrator_prev\n"); *1/ */
			/* /1* fprintf(stderr, "%+1.2f * %+1.2f + %+1.2f * %+1.2f + %+1.2f\n",Kp,Pe,Ki,Pe,Pintegrator_prev); *1/ */
			/* loop_filter_output = (Kp * Pe) + (Ki * Pe + Pintegrator_prev); // proportional + integrator */
			/* Pintegrator_prev = Ki * Pe + Pintegrator_prev; */

			/* out = lpf1+I*lpf2; */

			fwrite(&out,sizeof(complex float),1,stdout);
		}
		else
		{
			usleep(1000);
		}
	}
	return 0;
}
