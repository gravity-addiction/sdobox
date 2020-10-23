#!/usr/bin/env bash
action=$1
ifname=$2
id=$3
syspath=$4

echo "Net ${ifname} (${id}) ${action}" >> /tmp/iftest