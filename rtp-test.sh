#!/bin/bash

#Gstreamer1.0 working example UYVY streaming
#===========================================
echo 'Starting example application...'
./build/rtp-example &

sleep 1

echo 'Starting reciever...'

gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! timeoverlay ! videoconvert ! ximagesink sync=false &

sleep 10

pkill -f rtp-example
pkill -f gst-launch-1.0
exit





