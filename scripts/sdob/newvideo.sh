#!/usr/bin/env bash

# ./newvideo.sh -h zero.local -s 2020_cf_ghost_nationals -t 1002 -r 4 -d /home/pi/Videos -f 1302.mp4
POSITIONAL=()
HOSTER=1

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

while IFS= read -r HOST; do
  curl --header "Content-Type: application/json" --request POST --data '{"host":"'"${HOSTER}"'","slug":"'"${SLUG}"'","videoId":"'"${VIDEOID}"'","eventId":"'"${EVENTID}"'","compId":"'"${COMPID}"'","comp":"'"${COMP}"'","team":"'"${TEAM}"'","rnd":"'"${RND}"'","folder":"'"${FOLDER}"'","file":"'"${FILE}"'","url":"'"${URL}"'"}' "http://${HOST}:4004/p/skydiveorbust/newvideo"
done < "child_hosts"