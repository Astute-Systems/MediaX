#!/bin/bash

IP_ADDRESS=127.0.0.1

echo 'Starting example application...'
./build/bin/rtp-transmit -mode=4 -source=2 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004 &

sleep 1

echo 'Starting receiver...'

./build/bin/rtp-receive -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004 &

sleep 10

pkill -f rtp-transmit 
pkill -f rtp-receive
exit





