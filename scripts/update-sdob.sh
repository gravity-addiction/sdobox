#!/usr/bin/env bash

cd /home/pi/Downloads/sdobox/
git pull
make -j4
sudo systemctl stop sdobox.service
cp -r bin /opt/sdobox/
cp -r scripts /opt/sdobox/
cp -r images /opt/sdobox/
sudo systemctl start sdobox.service