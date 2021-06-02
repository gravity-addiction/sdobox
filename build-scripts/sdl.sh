#!/usr/bin/env bash

# Get full path to this folder
PWDSRC=`dirname "$(readlink -f "$0")"`
cd "${PWDSRC}/../SDL"

git submodule update --init

./autogen.sh
./configure --enable-sndio=no --enable-directfb=no

make clean && make -j4
cp build/libSDL* ../src/libs/SDL/
cp build/.libs/libSDL*.a ../src/libs/SDL/
