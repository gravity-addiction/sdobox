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

# Get full path to this folder
PWDSRC=`dirname "$(readlink -f "$0")"`
cd "${PWDSRC}/../SDL"

#git submodule update --init

if [ ! -z $makeonly ]; then
  ./autogen.sh
  ./configure --enable-sndio=no --enable-directfb=no
fi

make clean && make -j4
cp build/libSDL* ../src/libs/SDL/
cp build/.libs/libSDL*.a ../src/libs/SDL/

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
