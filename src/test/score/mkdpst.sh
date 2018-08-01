#!/bin/sh
set -e
set -x

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT'
# Profiling
#FLAGS='-DNDEBUG -pg'
# Normal
#FLAGS='-DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../../demo/utils -I../../dp -I../../tca -I../../unix/server -I../../score dpscoret.c ../../../demo/utils/rawlin.c ../../../linux/lib/dp2s.a -lgdbm -lefence -o dpscort
