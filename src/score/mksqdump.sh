#!/bin/sh
set -e
set -x
rm -rf sqdump 

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT'
# Profiling
#FLAGS='-DNDEBUG -pg'
# Normal
#FLAGS='-DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir sqdump
cd sqdump
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I.. ../sqdump.c ../sq.c ../wmq.c ../wmqfile.c ../scorerep.c ../../dp/assoctab.c ../../dp/dynatab.c -o sqdump
cd ..
