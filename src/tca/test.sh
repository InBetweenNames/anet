#!/bin/sh
set -e
set -x
rm -rf tcat

# Debugging + logging
#FLAGS = -DDEBUG -g -D_DEBUG
# Logging
#FLAGS = -DDPRNT
# Profiling
#FLAGS = -DNDEBUG -pg
# Normal
FLAGS='$CFLAGS -DNDEBUG'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir tcat
cd tcat
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I../../../demo/utils ../tcat.c ../tca.c ../tcapw.c ../crypttab.c ../../3rdparty/d3des/d3des.c ../../3rdparty/md5/md5c.c ../../dp/assoctab.c ../../dp/dynatab.c ../../../demo/utils/eclock.c ../../dp/ptimer.c ../../../demo/utils/mywcs.c

tcat
