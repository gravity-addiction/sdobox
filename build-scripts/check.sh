#!/usr/bin/env bash


# Check /boot/config
if ! grep -Fq "=op,dh" /boot/config.txt
then
  echo "No Fan Entry in /boot/config.txt"
fi

# waveshare overlays
if [ ! -f "/boot/overlays/waveshare35a.dtbo" ] && [ ! -f "/boot/overlays/waveshare35c.dtbo" ]; then
  echo "No waveshare35 overlays in /boot/overlays"
  # sudo cp scripts/overlays/* /boot/overlays/
fi

# Check for all sdobox system folders
if [ ! -d "/tmp" ]; then
  echo "No System tmp folder! /tmp"
fi

if [ ! -d "/opt/sdobox" ]; then
  echo "No Root System Folder /opt/sdobox"
  # sudo mkdir -p /opt/sdobox
else
  if [ ! -d "/opt/sdobox/images" ]; then
    echo "No SDOBOX System Images /opt/sdobox/images"
    # sudo cp -R images /opt/sdobox/
  fi
  if [ ! -d "/opt/sdobox/scripts" ]; then
    echo "No SDOBOX System Scripts /opt/sdobox/scripts"
    # sudo cp -R scripts /opt/sdobox/
  fi
  if [ ! -d "/opt/sdobox/tmp" ]; then
    echo "No SDOBOX System Images /opt/sdobox/tmp"
    # sudo mkdir -p /opt/sdobox/tmp
  fi
fi

if [ ! -f "/etc/udev/rules.d/95-ads7846.rules" ]; then
  echo "No udev rules for ads7846"
fi

if [ ! -f "/lib/systemd/system/mpv.service" ]; then
  echo "No MPV Service installed"
fi







