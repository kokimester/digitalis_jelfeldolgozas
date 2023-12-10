#!/bin/sh
#
#
gcc main.c -o decim -lm

cat dcf77_bb.bin |\
./decim > bb_decim.bin

# python3 plot.py bb_decim.bin 126
