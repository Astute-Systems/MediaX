#!/bin/bash -e

## For multicast use the following IP address
# IP_ADDRESS=239.192.1.1
IP_ADDRESS=127.0.0.1
PORT=5004

# RTP H.264 Video Stream encode from test source
gst-launch-1.0 \
    videotestsrc ! \
    video/x-raw,width=1280,height=720,framerate=50/1 ! \
    vaapih264enc keyframe-period=1 max-bframes=0 ! \
    rtph264pay ! \
    udpsink host=${IP_ADDRESS} port=${PORT} &

sleep 10

# Kill all the gstremers launch pipelines
pkill -f gst-launch-1.0

