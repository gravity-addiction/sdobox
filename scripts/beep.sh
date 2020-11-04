#!/usr/bin/env bash

( speaker-test -t sine -f 1000 -l 1 )& pid=$! ; sleep 0.4s ; kill -9 $pid