#!/bin/bash -e

echo "Starting 5 streames to run for $FRAMES frames each"

FRAMES=1000

# start a RAW UYUV gstreamer UDP video stream on port 5004
echo "Started stream 1 on 127.0.0.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="127.0.0.1" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 127.0.0.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="127.0.0.1" port=5005 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 127.0.0.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="127.0.0.1" port=5006 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 127.0.0.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="127.0.0.1" port=5007 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 127.0.0.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="127.0.0.1" port=5008 auto-multicast=true > /dev/null 2>&1 &


