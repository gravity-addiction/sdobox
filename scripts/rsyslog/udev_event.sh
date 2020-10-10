#!/usr/bin/env bash

SOCKETFILE=/tmp/sdobox.socket
if [ -S "$SOCKETFILE" ]; then
data=$(lsblk -o name,label,fstype,size,mountpoint --json)
json='{"event":"usb-drive","data":'"${data}"'}'
echo "${json}" | socat - "${SOCKETFILE}"
fi