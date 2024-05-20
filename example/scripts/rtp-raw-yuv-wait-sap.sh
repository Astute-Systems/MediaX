#!/bin/bash

IP_ADDRESS=127.0.0.1

echo 'Starting example application...'
./build/bin/rtp-transmit -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting reciever...'

./build/bin/rtp-receive &

sleep 10

pkill -f rtp-transmit 
pkill -f rtp-receive
exit





