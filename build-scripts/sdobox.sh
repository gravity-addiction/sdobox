#!/bin/bash
makeonly=
init=

while getopts ":ai" opt; do
    case $opt in
    a) makeonly="true" ;; # Handle -a
    i) init="true" ;; # Handle -i
    \?) ;; # Handle error: unknown option or missing required argument.
    esac
done

dir=$(pwd)
filedir=$(dirname $(realpath $0))
if [[ "$dir" == "$filedir" ]]; then
  cd ..
fi

if [ ! -z $init ]; then
  git submodule init
  git submodule update

  # Install Extra SDOBOX Libraries
  sudo apt install -y autoconf libtool libtool-bin libsdl-ttf2.0-0 libts0 libconfig9 fonts-noto-mono git libulfius2.5 libulfius-dev libxdo-dev libconfig-dev libsdl-ttf2.0-dev libsqlite3-dev libiw-dev libmpv-dev wmctrl uwsc libulfius-dev libavahi-client-dev libavahi-common-dev libavahi-common-data libavahi-glib1 basez

fi

if [ -z $makeonly ]; then
  make clean
fi

make -j4
