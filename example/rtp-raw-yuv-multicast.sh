#!/bin/bash

IP_ADDRESS=239.192.1.2

echo 'Starting example application...'
./build/bin/rtp-transmit -num-frames=250 -mode=1 -source=2 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

echo 'Starting receiver...'
./build/bin/rtp-receive -num-frames=250 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &


