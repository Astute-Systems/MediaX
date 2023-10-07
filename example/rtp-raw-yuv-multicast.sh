#!/bin/bash

IP_ADDRESS=239.192.1.2

echo 'Starting example application...'
./build/bin/transmit-example -mode=1 -source=2 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting receiver...'

./build/bin/receive-example -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 10

pkill -f transmit-example 
pkill -f receive-example
exit





