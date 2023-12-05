#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <malloc.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <curses.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

// Page 68 of RDS Standard
// ABCc'D
const char* offset_names = "ABCcD";

const uint8_t offset_positions[5] = {
	0,
	1,
	2,
	2,
	3
};

const uint16_t offset_words[5] = {
	0x00FC,
	0x0198,
	0x0168,
	0x0350,
	0x01B4
};

// ABCc'D
const uint16_t syndrome_words[5] = {
	0x017F,
	0x000E,
	0x012F,
	0x01EC,
	0x0197
};

const char* pty_strings[32]={
"Undefined",
"News",
"Information",
"Sports",
"Talk",
"Rock",
"Classic Rock",
"Adult Hits",
"Soft Rock",
"Top 40",
"Country",
"Oldies",
"Soft",
"Nostalgia",
"Jazz",
"Classical",
"Rhythm & Blues",
"Soft Rhythm & Blues",
"Language",
"Religious Music",
"Religious Talk",
"Personality",
"Public",
"College",
"Spanish Talk",
"Spanish Music",
"Hip Hop",
"Unassigned",
"Unassigned",
"Weather",
"Emergency Test",
"Emergency"
};
static char radio_text[65] = {0x00};
static char station_name[9] = {0x00};
const uint32_t register_mask = 0x03FFFFFF;
static uint8_t AB_flag_change = 0x00;

uint16_t calculate_syndrome(uint32_t message, uint8_t message_length);
void load_bit(uint32_t* shift_register, uint8_t bit);
void parse_rds(uint16_t data[4], uint8_t offsets[4]);
void debug_print(const char*);
void decode_radio_text(uint16_t data[4], bool is_a);

//page 64
uint16_t calculate_syndrome(uint32_t message, uint8_t message_length)
{
	uint64_t reg = 0x00;	
	uint32_t i;
	//					  0b 48db 0, 0000 0101 1010 1001
	const uint64_t poly = 0x5B9;
	const uint8_t plength = 10;
	for (i = message_length; i > 0; i--)  {
		reg = (reg << 1) | ((message >> (i-1)) & 0x01);
		if (reg & (1 << plength)) reg = reg ^ poly;
	}
	for (i = plength; i > 0; i--) {
		reg = reg << 1;
		if (reg & (1<<plength)) reg = reg ^ poly;
	}
	return (reg & ((1<<plength)-1));
}

void load_bit(uint32_t* shift_register, uint8_t bit)
{
	//bit must be either 0x00 or 0x01
	bit &= 0x01;
	*shift_register <<= 1;
	*shift_register |= bit;
	*shift_register &= register_mask;
}

void parse_rds(uint16_t data[4], uint8_t offsets[4])
{
	//2nd group first 4 bits
	uint16_t group_type = (data[1] >> 12) & 0x000f;
	//2nd group 5th bit
	bool is_b = (data[1] >> 11) & 0x1;
	uint16_t pi;
	uint8_t pty;
	pi = data[0];
	pty = (data[1] >> 5) & 0x1f;
	uint8_t station_name_index = data[1] & 0x03;
	station_name[station_name_index * 2]		= data[3] >> 8 & 0xff;
	station_name[station_name_index * 2 + 1]	= data[3] 		& 0xff;
	clear();
	printw("PI: %u \nSTATION: %s\n",pi,station_name);
	//printw("group type: %u%c\n",group_type, (is_b ? 'B' : 'A'));
	printw("Program: %s\n", pty_strings[pty]);
	printw(radio_text);
	refresh();
	if(group_type == 2)
	{
		decode_radio_text(data,is_b);
	}
}

void decode_radio_text(uint16_t* data, bool is_b)
{
	uint8_t ts_address = data[1] & 0x0f; 
	if(AB_flag_change != ((data[1] >> 4) & 0x01))
	{
		fprintf(stderr, "---reseting radio_text!---\n");
		memset(radio_text,' ',sizeof(radio_text));
	}	
	AB_flag_change = ((data[1] >> 4) & 0x01);
	if(!is_b)
	{
		radio_text[ts_address * 4] = (char)((data[2] >> 8) & 0xff);
		radio_text[ts_address * 4 + 1] = (char)((data[2]) & 0xff);
		radio_text[ts_address * 4 + 2] = (char)((data[3] >> 8) & 0xff);
		radio_text[ts_address * 4 + 3] = (char)((data[3]) & 0xff);
	}
	else
	{
		radio_text[ts_address * 2] = (char)((data[3] >> 8) & 0xff);
		radio_text[ts_address * 2 + 1] = (char)((data[3]) & 0xff);
	}

}

int main(int argc, char** argv)
{
	initscr();
	cbreak();
	noecho();
	clear();
	unsigned char in=0x00;
	bool is_synced = false;
	bool pre_sync_state = false; 
	bool group_can_be_built = false;

	uint16_t group[4] = {0x00};
	uint8_t offsets[4] = {0x00};

	uint32_t shift_register = 0x00;
	uint64_t bit_distance = 0;
	uint64_t block_distance = 0;

	uint8_t block_bit_counter= 0x00;
	uint64_t bit_counter= 0x00;
	uint8_t good_block_in_group_counter = 0x00;

	uint32_t blocks_counter = 0x00;
	uint32_t wrong_blocks = 0x00;
	uint8_t block_number = 0x00;
	
	uint8_t offset_character = 0x00;

	uint32_t last_seen_offset = 0x00;
	uint64_t last_seen_bit_counter= 0x00;
	while(1)
	{
		int k = fread(&in,sizeof(unsigned char),1,stdin);
		if(feof(stdin))
		{
			break;
		}
		if(k>0)
		{
			load_bit(&shift_register, in);
			if(is_synced)
			{
				//true if enough bits arrived
				if(++block_bit_counter > 25)
				{
					//debug_print("received 26 bits\n");
					bool is_block_good = false;
					//first 16 bits
					uint16_t data = (shift_register >> 10) & 0xFFFF;
					//last 10 bits
					uint16_t checkword = (shift_register) & 0x03FF;

					uint16_t calculated_block_syndrome = calculate_syndrome(data,16);
					uint16_t received_block_syndrome = 0x00;
					// C or C'
					if(block_number == 2)
					{
						received_block_syndrome = checkword ^ offset_words[block_number];
						if(received_block_syndrome == calculated_block_syndrome)
						{
							//we received a C block
							offset_character = 'C';
							is_block_good = true;
						}
						else
						{
							//we might have a c block
							received_block_syndrome = checkword ^ offset_words[3]; 
							if(received_block_syndrome == calculated_block_syndrome)
							{
								//we received a c block
								offset_character = 'c';
								is_block_good = true;
							}
							else
							{
								wrong_blocks++;
								is_block_good = false;
							}
						}
					}
					else
					{
						received_block_syndrome = checkword ^ offset_words[block_number]; 
						if(received_block_syndrome == calculated_block_syndrome)
						{
							is_block_good = true;
							switch(block_number)
							{
								case 0: offset_character = 'A'; break;
								case 1: offset_character = 'B'; break;
								case 3: offset_character = 'D'; break;
								default: offset_character = 0x00; //error
							};
						}
						else
						{
							wrong_blocks++;
							is_block_good=false;
						}
					}
					//we have checked if we have good block or not, lets decode
					//we start a group with 'A' block, so its 0
					if(block_number == 0 && is_block_good)
					{
						group_can_be_built = true;
						good_block_in_group_counter = 1;
					}
					if(group_can_be_built)
					{
						//should not happen, but just in case
						if(is_block_good == false)
						{
							group_can_be_built = false;
						}
						group[block_number] = data;
						offsets[block_number] = offset_character;
						good_block_in_group_counter++;
						if(good_block_in_group_counter >= 5)
						{
							//debug_print("parsing rds...\n");
							parse_rds(group,offsets);
						}
					}
					block_bit_counter = 0x00;
					block_number = (block_number + 1) % 4;
					blocks_counter++;
					if(blocks_counter >= 50)
					{
						//lost sync
						if(wrong_blocks >= 35)
						{
							//fprintf(stderr, "LOST SYNC, TOO MANY WRONG BLOCKS!\n");
							pre_sync_state = false;
							is_synced = false;
						}
						else
						{
							//fprintf(stderr, "STILL SYNCED, BAD BLOCKS: %u in %u blocks\n",wrong_blocks, blocks_counter);
						}
						blocks_counter=0x00;
						wrong_blocks=0x00;
					}
				}

			}
			//not yet synced
			else
			{
				uint16_t calculated_syndrome  = calculate_syndrome(shift_register,26);
				//checking which syndrome it matches
				for(uint32_t i=0; i<5; ++i)
				{
					if(calculated_syndrome == syndrome_words[i])
					{
						//debug_print("FOUND SYNDROME!\n");
						if(pre_sync_state == false)
						{
							last_seen_offset = i;
							last_seen_bit_counter = bit_counter;
							pre_sync_state = true;
						}
						else
						{
							//if we found a syndrome already, and find one again
							//calculate the distance between the detections
							bit_distance = bit_counter - last_seen_bit_counter;
							//if we found a block
							//calculate the distance
							if(offset_positions[last_seen_offset]>=offset_positions[i])
							{
								block_distance = offset_positions[i]+4-offset_positions[last_seen_offset]; 
							}
							else
							{
								block_distance = offset_positions[i] - offset_positions[last_seen_offset];	
							}
							//if the blocks werent equally spaced, we are not in pre_sync state
							if((block_distance*26 != bit_distance))
							{
								pre_sync_state = false;
							}
							else
							{
								//debug_print("SYNC STATE DETECTED\n");
								wrong_blocks  		= 0;
								blocks_counter     	= 0;
								block_bit_counter  	= 0;
								block_number      	= (i + 1) % 4;
								group_can_be_built	= false;
								is_synced          	= true;
							}

						}
					break; //syndrome found no need to search more
					}
				}
			}
			bit_counter++;
		}
		else
		{
			usleep(1000);
		}
	}
	endwin();	
	return 0;
}
