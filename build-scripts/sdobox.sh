#!/bin/bash

# Get full path to this folder
PWDSRC=`dirname "$(readlink -f "$0")"`

## Update API Utilities
./api-install.sh

cd "${PWDSRC}/.."
make clean
make -j4
