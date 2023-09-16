#!/bin/bash -e

echo "Starting 5 streames to run for $FRAMES frames each"
# pkill -f gst-launch-1.0 

FRAMES=1000

# start a RAW UYUV gstreamer UDP video stream on port 5004
echo "Started stream 1 on 329.192.3.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="239.192.3.1" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.2"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="239.192.3.2" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.3"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="239.192.3.3" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.4"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="239.192.3.4" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.5"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=10/1 ! videoconvert ! video/x-raw,format=UYVY ! rtpvrawpay ! udpsink host="239.192.3.5" port=5004 auto-multicast=true > /dev/null 2>&1 &

# GStreamer display the five streams
# echo "Displaying first streams"
# gst-launch-1.0 -v udpsrc -v multicast-group="239.192.3.1" port=5004 auto-multicast=true \
#   caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! \
#   rtpvrawdepay ! \
#   videoconvert ! \
#   xvimagesink sync=false 
