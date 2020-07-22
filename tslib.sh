#!/bin/sh
init=false

cd tslib

./autogen.sh
./configure
make -j4

while getopts "i:" opt; do
    case $opt in
    i) init=true ;; # Handle -a
    \?) ;; # Handle error: unknown option or missing required argument.
    esac
done

if [ ($init) ]; then
    sudo make install
    sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
else
  read -p "System Install (sudo make install)? " -n 1 -r
  echo    # (optional) move to a new line
  if [[ ! $REPLY =~ ^[Yy]$ ]]
  then
    sudo make install
    sudo cp -P /usr/local/lib/libts* /usr/lib/arm-linux-gnueabihf/
  fi
fi

cd ..