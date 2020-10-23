#!/usr/bin/env bash

echo 'got here' >> /tmp/dub.txt
if [[ "$1" == "dub:"* ]]; then
  ref=${1#dub://}
  /opt/sdobox/scripts/dubbing/dubbing.py "${ref}"
else
  xdg-open "$1"
fi