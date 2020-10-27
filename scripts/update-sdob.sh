#!/usr/bin/env bash

cd /home/pi/Downloads/sdobox/
git pull
make -j4
sudo systemctl stop sdobox.service
rsync -avs bin /opt/sdobox/
rsync -avs scripts /opt/sdobox/
rsync -avs images /opt/sdobox/
sudo systemctl start sdobox.service