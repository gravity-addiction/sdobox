#!/bin/bash

PART=$1


FS_LABEL=`lsblk -o name,label,fstype,size,mountpoint --json`
echo "${FS_LABEL}"