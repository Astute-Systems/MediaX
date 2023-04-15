#!/bin/bash

#Gstreamer1.0 working example UYVY streaming
#===========================================
gst-launch-1.0 videotestsrc num_buffers=500 ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! rtpvrawpay ! udpsink host=127.0.0.1 port=5004 &

sleep 1

echo 'Starting reciever...'

gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! videoconvert ! ximagesink sync=false
exit








gst-launch-1.0 videotestsrc num_buffers=1 ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! rtpvrawpay ! udpsink host=127.0.0.1 port=5004



gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false


gst-launch-1.0 videotestsrc pattern=1 ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! tee name=t ! queue ! rtpvrawpay ! queue ! udpsink bind-address=127.0.0.1 bind-port=5004 t. ! queue ! xvimagesink

gst-launch-1.0 videotestsrc ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue ! xvimagesink sync=false



gst-launch-1.0 videotestsrc ! video/x-raw, framerate=25/1, width=640, height=4800 ! ximagesink
