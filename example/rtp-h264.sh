#!/bin/bash

IP_ADDRESS=127.0.0.1

./build/bin/rtp-receive -num-frames=250 -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=25 -port=5004 &
# Save the PID of the process
PID=$!

./build/bin/rtp-transmit -verbose -num-frames=250 -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=25 -port=5004 -source=2

# Wait for rtp-receive to complete
wait $PID