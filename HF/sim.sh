#!/bin/sh

gcc costas.c -o costas -lm
gcc decimator.c -o decimator -lm
gcc symsync.c -o symsync -lm
gcc rds_decoder.c -o rds_decoder -lm -lncurses

fs=19000

cat tocostas.cf32 |\
./costas $fs |\
tee fromcostas.cf32 |\
./symsync 16 |\
./rds_decoder
echo "done"
# python3 plot.py fromcostas.cf32 fromsync.cf32
