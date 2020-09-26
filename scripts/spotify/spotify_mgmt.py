#!/usr/bin/env python

import json
import requests
import os


def spotify_request(method, url):
  f = open('/tmp/spotify.json',) 
  data = json.load(f)
  f.close()

  headers = {
  'Authorization': 'Bearer ' + data['access_token'],
  'Content-Type': 'application/json; charset=utf-8'
  }
  r = requests.request(method, url, headers=headers)
  return r.json()

def spotify_refresh_token():
  os.system('/opt/sdobox/scripts/spotify/register_device_refresh.sh')


userInfo = spotify_request('GET', 'https://api.spotify.com/v1/me')
if 'error' in userInfo:
  spotify_refresh_token()
else:
  print(userInfo['display_name'])

