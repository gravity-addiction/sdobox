#!/usr/bin/env bash

while read input
do

JSOURCE=$(echo "$input" | jq --raw-output '.source')
MESSAGE=$(echo "$input" | jq --raw-output '.message')

if (
  [ "$JSOURCE" == "kernel" ] && (
    [[ "$MESSAGE" == *"USB disconnect"* ]] || 
    # [[ "$MESSAGE" == *"New USB device found"* ]] || 
    [[ "$MESSAGE" == *"mounted filesystem"* ]]
  )
) || (
  [ "$JSOURCE" == "udisksd" ] && (
    [[ "$MESSAGE" == *"Mounted"* ]] ||
    [[ "$MESSAGE" == *"Unmounted"* ]]
  )
); then
  ### USB Drive Event
  SOCKETFILE=/tmp/sdobox.socket
  if [ -S "$SOCKETFILE" ]; then
  sleep .2
  data=$(lsblk -o name,label,fstype,size,mountpoint --json)
  json='{"event":"usb-drive","syslog":'"${input}"',"data":'"${data}"'}'
  echo "${json}" | socat - "${SOCKETFILE}" > /dev/null 2>&1
  fi

fi

echo "OK"

done
echo 0