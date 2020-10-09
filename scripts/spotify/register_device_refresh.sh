#!/usr/bin/env bash

SPOTIFYJSON=/opt/sdobox/tmp/spotify.json

# If not spotify.json exists, grab brand new token
if [ -f "${SPOTIFYJSON}" ]; then
# /opt/sdobox/scripts/spotify/register_device_local.sh

# Grab client_id from file
CLIENTID=$(cat /opt/sdobox/scripts/spotify/client)
TOKEN=$(jq -cer .refresh_token "${SPOTIFYJSON}")

# Refresh Token, Save Response Status in HTTPSTATUS
HTTPSTATUS=$(curl -H 'Content-Type: application/x-www-form-urlencoded' \
--write-out '%{http_code}' \
--silent \
-d 'grant_type=refresh_token' \
-d "refresh_token=${TOKEN}" \
-d "client_id=${CLIENTID}" \
-o /opt/sdobox/tmp/spotify.json \
https://accounts.spotify.com/api/token)

# Bad Response, Try Getting Brand New Token
if [ "${HTTPSTATUS}" -gt 299 ]; then
rm "${SPOTIFYJSON}"
/opt/sdobox/scripts/spotify/register_device_local.sh
fi
fi
