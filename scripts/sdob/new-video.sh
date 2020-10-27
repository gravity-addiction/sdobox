#!/usr/bin/env bash

# ./new-video.sh -h zero.local -s 2018_uspa_canopy_nationals -c "2way Seq Open" -t 3201 -r 1 -es "cf2WaySequentials" -d /home/pi/Videos -f 2_28_3201_1_1080p.m3u8
POSITIONAL=()
HOSTER=1
ES="fs"

while [[ $# -gt 0 ]]
do
KEY="$1"
case "${KEY}" in
  --hoster)
  HOSTER="$2"
  shift
  shift
  ;;
  -s|--slug)
  SLUG="$2"
  shift
  shift
  ;;
  -es|--eventsettings)
  ES="$2"
  shift
  shift
  ;;
  -v|--videoid)
  VIDEOID="$2"
  shift
  shift
  ;;
  -e|--eventid)
  EVENTID="$2"
  shift
  shift
  ;;
  -c|--compid)
  COMPID="$2"
  shift
  shift
  ;;
  --comp)
  COMP="$2"
  shift
  shift
  ;;
  -t|--team)
  TEAM="$2"
  shift
  shift
  ;;
  -r|--round)
  RND="$2"
  shift
  shift
  ;;
  -d|--directory)
  FOLDER="$2"
  shift
  shift
  ;;
  -f|--file)
  FILE="$2"
  shift
  shift
  ;;
  -u|--url)
  URL="$2"
  shift
  shift
  ;;
  -h|--host)
  HOST="$2"
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
  curl -s --header "Content-Type: application/json" --request POST --data '{"host":"'"${HOSTER}"'","slug":"'"${SLUG}"'","videoId":"'"${VIDEOID}"'","eventId":"'"${EVENTID}"'","compId":"'"${COMPID}"'","comp":"'"${COMP}"'","team":"'"${TEAM}"'","rnd":"'"${RND}"'","folder":"'"${FOLDER}"'","file":"'"${FILE}"'","url":"'"${URL}"'","es":"'"${ES}"'"}' "http://${HOST}:4004/p/skydiveorbust/newvideo" &> /dev/null
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