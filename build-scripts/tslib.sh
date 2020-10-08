#!/bin/bash
init=

cd ..

git submodule init
git submodule update

cd tslib

./autogen.sh
./configure
make -j4

while getopts ":i" opt; do
    case $opt in
    i) init="true" ;; # Handle -a
    \?) ;; # Handle error: unknown option or missing required argument.
    esac
done

if [  ! -z $init ]; then
    sudo make install
    sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
else
  read -p "System Install (sudo make install)? y/N " -n 1 -r
  echo    # (optional) move to a new line
  if [[ ! $REPLY =~ ^[Yy]$ ]];
  then
    echo
  else
    sudo make install
    sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
  fi
fi

cd ..