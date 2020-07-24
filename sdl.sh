#!/bin/bash
init=
makeonly=

while getopts ":ia" opt; do
    case $opt in
    a) makeonly="true" ;; # Handle -a
    i) init="true" ;; # Handle -a
    \?) ;; # Handle error: unknown option or missing required argument.
    esac
done

git submodule init
git submodule update

cd SDL
if [ ! -z $makeonly ]; then
  ./autogen.sh
  ./configure --enable-sndio=no
fi

make clean && make -j4
cp build/libSDL* ../libs/SDL/
cp build/.libs/libSDL*.a ../libs/SDL/



if [ ! -z $init ]; then
#  sudo make install
  echo
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
