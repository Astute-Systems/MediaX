#!/bin/bash

IP_ADDRESS=127.0.0.1

echo 'Starting example application...'
./build/bin/rtp-transmit -num-frames=250 -mode=2 -source=3 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

echo 'Starting receiver...'
./build/bin/rtp-receive -num-frames=250 -mode=2 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &





