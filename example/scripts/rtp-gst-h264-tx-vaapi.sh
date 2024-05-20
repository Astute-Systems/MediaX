#!/bin/bash -e

## For multicast use the following IP address
# IP_ADDRESS=239.192.1.1
IP_ADDRESS=127.0.0.1
PORT=5004

# Transmit H.264 RTP stream from test source
./build/bin/rtp-transmit  -source=2 -mode=4 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=${PORT}  &

# RTP H.264 Video Stream decode using Intel VAAPI
gst-launch-1.0 \
    udpsrc caps="application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264,payload=(int)96" port=${PORT} ! \
    queue ! \
    rtph264depay ! \
    h264parse ! \
    queue ! \
    vaapih264dec ! \
    textoverlay text="RTP H.264 Video Stream" shaded-background=true ! \
    videoconvert ! \
    xvimagesink sync=false & 

# Kill all the gstremers launch pipelines
sleep 10

pkill -f rtp-transmit 
pkill -f gst-launch-1.0
exit

