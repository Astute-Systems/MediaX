#!/bin/bash

IP_ADDRESS=127.0.0.1
FRAMERATE=25

./build/bin/rtp-receive -verbose -num-frames=250 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=${FRAMERATE} -port=5004 &
# Save the PID of the process
PID=$!

./build/bin/rtp-transmit  -num-frames=250 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=${FRAMERATE} -port=5004 -source=11

# Wait for rtp-receive to complete
wait $PID







