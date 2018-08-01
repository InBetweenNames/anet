#!/bin/sh
set -e
set -x
rm -rf antpservd

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT'
# Profiling
#FLAGS='-DNDEBUG -pg'
# Normal
#FLAGS='-DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -I.. -I../../score -I../../../h"

mkdir antpservd
cd antpservd
gcc ${COPT} "-DPACK=__attribute__((packed)) __attribute__((ms_struct))" ../antpservd.c ../antpserv.c ../../score/wmqfile.c -o antpservd
cd ..
