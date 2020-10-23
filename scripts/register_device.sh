#!/usr/bin/env bash
# SERIAL=$(cat /proc/cpuinfo | grep Serial | cut -d ' ' -f 2)
patternSerial='^Serial.*([[:xdigit:]]{16})$'
patternModel='^Model.*: (.*)$'
patternRevision='^Revision.*: (.*)$'


# CPU Info
while read -r line
do
    if [[ $line =~ $patternSerial ]]
    then
        SERIAL="${BASH_REMATCH[1]}"
    fi
    if [[ $line =~ $patternModel ]]
    then
        MODEL="${BASH_REMATCH[1]}"
    fi
    if [[ $line =~ $patternRevision ]]
    then
        REVISION="${BASH_REMATCH[1]}"
    fi
done < /proc/cpuinfo


# Network Info
ethIp4="$(/sbin/ip -o -4 addr list eth0 | awk '{print $4}')"
ethIp6="$(/sbin/ip -o -6 addr list eth0 | awk '{print $4}')"
wifiIp4="$(/sbin/ip -o -4 addr list wlan0 | awk '{print $4}')"
wifiIp6="$(/sbin/ip -o -6 addr list wlan0 | awk '{print $4}')"
hostName="$(hostname)"
# echo "${SERIAL} - ${MODEL} - ${REVISION} - ${ethIp4} - ${ethIp6} - ${wifiIp4} - ${wifiIp6}"

curl -X "POST" -H "Content-Type: application/json" -d "{\"s\":\"${SERIAL}\",\"m\":\"${MODEL}\",\"r\":\"${REVISION}\",\"e4\":\"${ethIp4}\",\"e6\":\"${ethIp6}\",\"w4\":\"${wifiIp4}\",\"w6\":\"${wifiIp6}\",\"h\":\"$(hostname)\"}" -s -o /opt/sdobox/device.token https://dev.skydiveorbust.com/api/latest/rpi/code # https://spotify.skydiveorbust.com/rpi/code