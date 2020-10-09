#!/usr/bin/env bash
data=$(lsblk -o name,label,fstype,size,mountpoint --json)
json='{"event":"usb-drive-added","data":'"${data}"'}'
echo "${json}" | socat - /tmp/sdobox.socket