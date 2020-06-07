#!/bin/sh

cd SDL
./autogen.sh
./configure --enable-sndio=no
make clean && make -j4
cp build/libSDL* ../libs/SDL/
cp build/.libs/libSDL*.a ../libs/SDL/
cd ..