#!/usr/bin/env bash
json='{"event":"usb-drive-added"}'
echo "${json}" | socat - /tmp/sdobox.socket