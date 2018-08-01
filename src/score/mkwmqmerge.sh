#!/bin/sh
set -e
set -x
rm -rf wmqmerge 

# Debugging + logging
#FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT'
# Profiling
#FLAGS='-DNDEBUG -pg'
# Normal
FLAGS='$CFLAGS -DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir wmqmerge
cd wmqmerge
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I.. ../wmqmerge.c ../wmq.c ../wmqfile.c -o wmqmerge
cd ..
