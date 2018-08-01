#!/bin/sh
set -e
set -x
rm -rf wmqdump 

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT'
# Profiling
#FLAGS='-DNDEBUG -pg'
# Normal
#FLAGS='-DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir wmqdump
cd wmqdump
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I.. ../wmqdump.c ../wmq.c ../wmqfile.c -o wmqdump
cd ..
