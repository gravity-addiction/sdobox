#!/bin/bash
init=

git submodule init
git submodule update

cd SDL
./autogen.sh
./configure --enable-sndio=no
make clean && make -j4
cp build/libSDL* ../libs/SDL/
cp build/.libs/libSDL*.a ../libs/SDL/

while getopts ":i" opt; do
    case $opt in
    i) init="true" ;; # Handle -a
    \?) ;; # Handle error: unknown option or missing required argument.
    esac
done

if [ ! -z $init ]; then
  sudo make install
else
  read -p "System Install (sudo make install)? y/N " -n 1 -r
  echo    # (optional) move to a new line
  if [[ ! $REPLY =~ ^[Yy]$ ]];
  then
    echo
  else
    sudo make install
  fi
fi

cd ..