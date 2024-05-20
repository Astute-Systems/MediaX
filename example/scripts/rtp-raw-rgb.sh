#!/bin/bash

IP_ADDRESS=127.0.0.1

./build/bin/rtp-receive -verbose -num-frames=250 -mode=0 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004 &
# Save the PID of the process
PID=$!

./build/bin/rtp-transmit -num-frames=250 -mode=0 -source=2 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004 

# Wait for rtp-receive to complete
wait $PID




