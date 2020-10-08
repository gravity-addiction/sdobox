#!/usr/bin/env bash
json='{"event":"usb-drive-removed"}'
echo "${json}" | socat - /tmp/sdobox.socket