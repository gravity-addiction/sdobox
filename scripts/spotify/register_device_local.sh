#!/usr/bin/env bash

# Spotify Developer Client ID
CLIENTID=$(cat /opt/sdobox/scripts/spotify/client)

# Generate Spotify Codes
TLEN=$(( ( RANDOM % (128 - 43) )  + 43 ))
RCODE=$(cat /dev/urandom | tr -dc 'a-z0-9-_~.' | fold -w "${TLEN}" | head -n 1)

# Save Code to Tmp File
echo "${RCODE}" > /tmp/spotify.code
CODE=$(cat /tmp/spotify.code)

# Make Auth URL
URLCODE=$(echo -n "${CODE}" | sha256sum  | cut -f1 -d\  | xxd -r -p | base64url | sed -e "s/=//g" -e "s/\+/-/g" -e "s/\//_/g")
SPOTURL="https://accounts.spotify.com/authorize?client_id=${CLIENTID}&response_type=code&redirect_uri=http://localhost:4004/H3xx92sk&scope=user-modify-playback-state%20user-read-playback-state%20user-read-currently-playing"
SPOTURL="${SPOTURL}&code_challenge_method=S256&code_challenge=${URLCODE}"

# Open Browser to Auth
DISPLAY=:0.0 /usr/bin/chromium-browser ${SPOTURL} &
DISPLAY=:0.0 /usr/bin/wmctrl -a '- Chromium'
