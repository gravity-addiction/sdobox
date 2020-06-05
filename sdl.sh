#!/bin/sh

cd SDL
./autogen.sh
./configure --enable-sndio=no
make -j4
cp build/libSDL* ../sdobox/libs/SDL/
cp build/.libs/libSDL*.a ../sdobox/libs/SDL/
cd ..