#!/bin/bash

git submodule init
git submodule update

# Install Extra SDOBOX Libraries
sudo apt install -y autoconf libtool libtool-bin libsdl-ttf2.0-0 libts0 libconfig9 fonts-noto-mono git libulfius2.5 libulfius-dev libxdo-dev libconfig-dev libsdl-ttf2.0-dev libsqlite3-dev libiw-dev libmpv-dev wmctrl

if [ ! -f "~/.config/touchapp/touchapp.conf" ]; then
  mkdir -p ~/.config/touchapp

  echo 'rotary_pin_a = 24;
  rotary_pin_b = 25;
  rotary_pin_btn = 23;
  right_pin_btn = 22;
  left_pin_btn = 21;
  debounce_delay = 175;
  btn_hold_delay = 800;' | tee ~/.config/touchapp/touchapp.conf
fi

make -j4
