#!/usr/bin/env bash

# Accept Querystring 'code' as stdin input from webserver
CODEVERIFY=$1

# Fetch Stored Variables
CODEVERIFIER=$(cat /opt/sdobox/tmp/spotify.code)
CLIENTID=$(cat /opt/sdobox/scripts/spotify/client)

# Request Access Token from auth code, store reponse in /opt/sdobox/tmp/spotify.json
curl -X "POST" \
--silent \
-H 'Content-Type: application/x-www-form-urlencoded' \
-d "client_id=${CLIENTID}" \
-d 'grant_type=authorization_code' \
-d 'redirect_uri=http://localhost:4004/H3xx92sk' \
-d "code=${CODEVERIFY}" \
-d "code_verifier=${CODEVERIFIER}" \
-o /opt/sdobox/tmp/spotify.json \
https://accounts.spotify.com/api/token
