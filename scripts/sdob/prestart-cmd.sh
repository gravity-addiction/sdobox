#!/usr/bin/env bash

# ./newvideo.sh -h zero.local -s 2018_uspa_canopy_nationals -c "2way Seq Open" -t 3201 -r 1 -es "cf2WaySequentials" -d /home/pi/Videos -f 2_28_3201_1_1080p.m3u8
POSITIONAL=()

while [[ $# -gt 0 ]]
do
KEY="$1"
case "${KEY}" in
  -h|--host)
  HOST="$2"
  shift
  shift
  ;;
  -t|--time)
  STARTTIME="$2"
  shift
  shift
  ;;
  -wt|--workingtime)
  WORKINGTIME="$2"
  shift
  shift
  ;;
  *)
  POSITIONAL+=("$1")
  shift
esac
done

function notifyHost() {
  # echo "Notify: ${HOST}"
  curl -s --header "Content-Type: application/json" --request PUT --data '{"sopst":"'"${STARTTIME}"'","pst":"'"${WORKINGTIME}"'"}' "http://${HOST}:4004/p/skydiveorbust/prestart" &> /dev/null
}
if [ -z "$HOST" ]; then
  HOSTNAME=$(hostname -s)
  while IFS= read -r HOST; do
    if [[ ${HOST} != "${HOSTNAME}.local" ]]; then
      notifyHost
    fi
  done < "/opt/sdobox/scripts/sdob/child_hosts"

else
  notifyHost
fi