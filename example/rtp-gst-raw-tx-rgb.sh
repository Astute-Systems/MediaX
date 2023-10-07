#!/bin/bash

#IP_ADDRESS=238.192.1.1
IP_ADDRESS=127.0.0.1

#Gstreamer1.0 working example UYVY streaming
#===========================================

echo 'Starting example application...'
./build/bin/transmit-example  -source=2 -mode=0 -device=/dev/video1 -ipaddr=$IP_ADDRESS -height=480 -width=640 -port=5004  &

sleep 1

echo 'Starting reciever...'

gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)RAW, sampling=(string)RGB, depth=(string)8, width=(string)640, height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! timeoverlay ! videoconvert ! ximagesink sync=false &

sleep 10

pkill -f transmit-example
pkill -f gst-launch-1.0
exit





