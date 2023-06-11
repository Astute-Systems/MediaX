#!/bin/bash

#IP_ADDRESS=238.192.1.1
IP_ADDRESS=127.0.0.1

#Gstreamer1.0 working example UYVY streaming
#===========================================

echo 'Starting example application...'
./build/bin/transmit-example -filename images/testcard.png -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting reciever...'

./build/bin/receive-example -wait-sap=true &

sleep 10

pkill -f transmit-example 
pkill -f receive-example
exit





