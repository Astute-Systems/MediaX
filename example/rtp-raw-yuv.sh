#!/bin/bash

IP_ADDRESS=127.0.0.1
FREAMERATE=20

echo 'Starting receiver...'
./build/bin/rtp-receive -num-frames=250 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=${FREAMERATE} -port=5004 &
# Save the PID of the process
PID=$!

echo 'Starting example application...'
./build/bin/rtp-transmit -verbose -num-frames=250 -mode=1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=${FREAMERATE} -port=5004 -source=2

# Wait for rtp-receive to complete
wait $PID







