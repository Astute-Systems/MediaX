#!/bin/bash -e

FRAMES=1000

echo "Starting 5 streames to run for $FRAMES frames each"
# start a RAW UYUV gstreamer UDP video stream on port 5004
echo "Started stream 1 on 239.192.3.1"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay ! udpsink host="239.192.3.1" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.2"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay ! udpsink host="239.192.3.2" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.3"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay ! udpsink host="239.192.3.3" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.4"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay ! udpsink host="239.192.3.4" port=5004 auto-multicast=true > /dev/null 2>&1 &
echo "Started stream 1 on 239.192.3.5"
gst-launch-1.0 -v videotestsrc num-buffers=$FRAMES ! video/x-raw,format=UYVY,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! x264enc ! rtph264pay ! udpsink host="239.192.3.5" port=5004 auto-multicast=true > /dev/null 2>&1 &
