#!/bin/sh

gcc costas.c -o costas -lm
gcc decimator.c -o decimator -lm
gcc symsync.c -o symsync -lm
gcc rds_decoder.c -o rds_decoder -lm -lncurses
gcc throw1inN.c -o throw1inN -lm
gcc demod.c -o demod -lm

fs=19000

#works!!!
#solid_tocostas.cf32
#from gnuradio:
#tocostas.cf32
cat tocostas.cf32 |\
./costas $fs |\
tee fromcostas.cf32 |\
./decimator 8 |\
./symsync 2 |\
tee fromsync.cf32 |\
./demod |\
tee todecoder.bin |\
./rds_decoder > output.txt
echo "done"
# less costas.txt
# python3 plot.py fromcostas.cf32 grccostas.cf32
# python3 plot.py tocostas.cf32 fromcostas.cf32 
# python3 plot.py fromcostas.cf32 fromsync.cf32
