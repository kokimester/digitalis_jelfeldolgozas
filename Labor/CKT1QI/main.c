#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

//converts the coded bits into a decimal number
uint8_t bintodec(uint64_t p,int start,size_t count,int* coefs){
	uint8_t dec = 0;
	uint64_t tmp = p>>start;
	for(size_t i = 0; i < count; ++i){
		uint8_t bit =(tmp>>i)&0x00000001;
		/* fprintf(stderr,"%u += %u * %d\n", dec,bit,coefs[i]); */
		dec += bit*coefs[i];
	}
	/* fprintf(stderr,"\n\n"); */
	return dec;
}

//calculates the XOR of all the bits specified by the parameters
uint8_t checksum(uint64_t p,int start,size_t count){
	uint8_t checksum = 0;
	uint64_t tmp = p>>start;
	for(size_t i = 0; i < count; ++i){
		checksum = (checksum ^ (tmp>>i)&0x00000001)&0x00000001;
	}
	return checksum;
}


void print_packet(uint64_t p){
	//coefficient arrays
	int mins[] = {1,2,4,8,10,20,40};
	int hours[] = {1,2,4,8,10,20};
	int weekdays[] = {1,2,4};
	int months[] = {1,2,4,8,10};
	int years[] = {1,2,4,8,10,20,40,80};
	//checksum calculations
	uint8_t hour_checksum = checksum(p,29,6);
	uint8_t minute_checksum = checksum(p,21,7);
	uint8_t year_checksum = checksum(p,36,22);
	//decoding data
	uint8_t minute = bintodec(p,21,7,mins);
	uint8_t hour = bintodec(p,29,6,hours);
	uint8_t day = bintodec(p,36,6,hours);
	uint8_t weekday = bintodec(p,42,3,weekdays);
	uint8_t month = bintodec(p,45,5,months); 
	uint8_t year = bintodec(p,50,8,years); 
	//weekdays as strings for pretty printing
	const char weekdays_str[][20] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday","Saturday","Sunday"};
	fprintf(stderr,"minute_checksum: %s\n",minute_checksum == ((p>>28)&0x00000001) ? "ok" : "not ok" );
	fprintf(stderr,"hour_checksum: %s\n",hour_checksum == ((p>>35)&0x00000001) ? "ok" : "not ok" );
	fprintf(stderr,"year_checksum: %s\n",year_checksum == ((p>>58)&0x00000001) ? "ok" : "not ok" );
	fprintf(stderr,"%s ",(p >> 17)&0x00000001 > 0 ? "CEST" : "CET");
	fprintf(stderr, "%02u:%02u 20%02u.%02u.%02u %s\n",hour,minute,year,month,day,weekdays_str[weekday-1]);
	
}

int main(int argc, char **argv)
{
	long n = 113;
	if (argc > 1)
	{
		n = atol(argv[1]);
		n = n > 0 ? n : -n;
	}
	float in=0.f, out=0.f;
	//fs = 14238 / 113 = 126
	//126 minta minden masodpercbol
	//0.1s alacsony szint  == 0 bit
	//0.1s = 12.6 minta ~ 12 minta
	//0.2s alacsony szint  == 0 bit
	//0.2s = 25.2 minta ~ 25 minta
	long count = 0;
	long low_counter = 0;
	long high_counter = 0;
	long bit_counter = 0;
	long output_counter = 0;
	uint64_t packet = 0;
	bool synced = false;
	const float THRESHOLD = 0.03f;
	const long BIT_THRESHOLD = 15;
	const long fs = 126;
	while (1)
	{
		int k = fread(&in, sizeof(float), 1, stdin);
		if (feof(stdin))
		{
			break;
		}
		if (k > 0)
		{
				count++;
				//decimating by n=113
				if (count >= n)
				{
					output_counter++;
					out = in;
					//signal dropped low
					//AM modulated signal
					if(out < THRESHOLD)
					{
						high_counter = 0;
						low_counter++;
					}else{
						high_counter++;
					}
					//signal came back up high
					//searching for possible sync window
					if(synced == false && high_counter >= fs){
						fprintf(stderr,"sync detected![%ld]\n",output_counter);
						synced = true;
					}
					//end of AM modulated part
					if((low_counter != 0) && (out > THRESHOLD)){
						if(synced){
							bit_counter++;
							packet <<= 1;
							//decode bit
							if(low_counter < BIT_THRESHOLD){
							//0
							/* fprintf(stderr,"[%u]0\n",bit_counter-1); */
							}else{
							//1
							/* fprintf(stderr,"[%u]1\n",bit_counter-1); */
							packet |= 0x00000001;
							}
						}
						//all of the bits from the packet have arrived
						if(bit_counter >= 59){
							/* fprintf(stderr,"\nend of packet\n"); */
							//need to invert the bits because
							//the decode logic is written in reverse
							uint64_t inverse_packet = 0;
							for(int i = 58; i>-1;--i){
								fprintf(stderr,"%c",((packet >> i) & 0x00000001) == 1 ? '1' : '0');
								inverse_packet |= (((packet >> i) & 0x00000001) << (58-i));
							}
							fprintf(stderr,"\n");

							print_packet(inverse_packet);

							//reseting variables for next sync window
							packet = 0;
							synced = false;
							bit_counter = 0;
						}
						low_counter = 0;
					}
					//reseting decimator
					count = 0;
					fwrite(&out, sizeof(float), 1, stdout);
				}
		}
		else
		{
			usleep(1000);
		}
	}

	return 0;
}
