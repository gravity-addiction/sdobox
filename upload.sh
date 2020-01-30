#!/bin/sh
/usr/bin/rsync -avz --no-perms --no-owner --no-group --exclude wpa/os_unix.o --exclude wpa/wpa_ctrl.o --delete --progress ../advert-gui/* $1:~/dev/advert-gui
