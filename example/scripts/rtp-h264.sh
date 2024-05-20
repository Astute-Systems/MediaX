#!/bin/bash

# Set the number of frames to transmit from $1
# If no argument is given, set it to 250
if [ -z "$1" ]
then
    NUM_FRAMES=250
else
    NUM_FRAMES=$1
fi

IP_ADDRESS=127.0.0.1

./build/bin/rtp-receive -num-frames=${NUM_FRAMES} -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=25 -port=5004 &
# Save the PID of the process
PID=$!

./build/bin/rtp-transmit -verbose -num-frames=${NUM_FRAMES} -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -framerate=25 -port=5004 -source=2

# Wait for rtp-receive to complete
wait $PID