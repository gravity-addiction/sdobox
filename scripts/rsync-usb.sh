#!/usr/bin/env bash

USBDRIVE=$1
/usr/bin/rsync -avz --progress "~/Videos3" "pi@sandyclaws.local:/media/pi/${USBDRIVE}/Videos"