#!/usr/bin/env bash

HOST=$1
/usr/bin/rsync -avz --no-perms --no-owner --no-group --progress /home/pi/Videos/2018/* $1:/home/pi/Videos/
