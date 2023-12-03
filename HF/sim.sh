#!/bin/sh

gcc costas.c -o costas -lm

fs=19000

cat tocostas.cf32 |\
./costas $fs > fromcostas.cf32
echo "done"
python3 plot.py tocostas.cf32 fromcostas.cf32
