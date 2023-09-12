#!/bin/bash

IP_ADDRESS=127.0.0.1

echo 'Starting example application...'
./build/bin/transmit-example -source=5 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting receiver...'

./build/bin/receive-example -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 10

pkill -f transmit-example 
pkill -f receive-example
exit




