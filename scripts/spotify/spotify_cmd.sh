#!/usr/bin/env bash

METHOD=$1
URL=$2

case "$1" in
  next )
    METHOD="POST"
    URL="player/next"
    ;;
  prev )
    METHOD="POST"
    URL="player/previous"
    ;;
  play )
    METHOD="PUT"
    URL="player/play"
    ;;
  pause )
    METHOD="PUT"
    URL="player/pause"
    ;;
  mute )
    METHOD="PUT"
    URL="player/volume?volume_percent=0"
    ;;
  unmute )
    METHOD="PUT"
    URL="player/volume?volume_percent=100"
    ;;
  volume )
    METHOD="PUT"
    URL="player/volume?volume_percent=$2"
    ;;
  seek )
    METHOD="PUT"
    URL="player/seek?position_ms=$2"
    ;;
  shuffle )
    METHOD="PUT"
    URL="player/shuffle?state=true"
    ;;
  noshuffle )
    METHOD="PUT"
    URL="player/shuffle?state=false"
    ;;

esac

if [ ! -z "${METHOD}" ] && [ ! -z "${URL}" ]; then

SPOTIFYJSON=/opt/sdobox/tmp/spotify.json
TOKEN=$(jq -cer .accessToken "${SPOTIFYJSON}")

HTTPSTATUS=$(curl -X "${METHOD}" \
--write-out '%{http_code}' \
--fail \
--silent \
-H "Authorization: Bearer ${TOKEN}" \
https://api.spotify.com/v1/me/${URL})

# echo "${HTTPSTATUS}"
if [ -f "${SPOTIFYJSON}" ] && [ "${HTTPSTATUS}" -gt 299 ]; then
  echo "Refreshing Spotify Token"
  node /opt/sdobox/scripts/spotify/spotify.js
  # Try Refreshing Token
  # /opt/sdobox/scripts/spotify/register_device_refresh.sh
  echo "Checking For Refreshments!"
  curl -X "${METHOD}" \
  --write-out '%{http_code}' \
  --silent \
  -H "Authorization: Bearer ${TOKEN}" \
  https://api.spotify.com/v1/me/${URL}
fi
fi

