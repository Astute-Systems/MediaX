#!/bin/bash -e

IP_ADDRESS=127.0.0.1
## For multicast use the following IP address
# IP_ADDRESS=239.192.1.1
PORT=5004

# RTP H.264 Video Stream encode from test source
gst-launch-1.0 \
    videotestsrc ! \
    video/x-raw,width=1920,height=1080,framerate=50/1 ! \
    nvh264enc bitrate=1000000 ! \
    rtph264pay ! \
    udpsink host=${IP_ADDRESS} port=${PORT} &
sleep 1

# RTP H.264 Video Stream decode using Intel VAAPI
gst-launch-1.0 -v \
    udpsrc caps="application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264,payload=(int)96" port=${PORT} ! \
    queue ! \
    rtph264depay ! \
    h264parse ! \
    queue ! \
    nvh264dec ! \
    textoverlay text="RTP H.264 Video Stream" shaded-background=true ! \
    videoconvert ! \
    glimagesink sync=false & 

sleep 1
echo "Waiting for 10 seconds to receive RTP stream"
sleep 10

# Kill all the gstremers launch pipelines
pkill -f gst-launch-1.0

# List all the available options on the encoder See https://ottverse.com/i-p-b-frames-idr-keyframes-differences-usecases/
gst-inspect-1.0 vaapih264enc
