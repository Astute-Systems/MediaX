#!/bin/bash

IP_ADDRESS=127.0.0.1

echo 'Starting example application...'
./build/bin/transmit-example -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting reciever...'

./build/bin/receive-example &

sleep 10

pkill -f transmit-example 
pkill -f receive-example
exit




