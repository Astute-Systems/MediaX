#!/bin/bash -e

gst-launch-1.0 -v udpsrc caps="application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264" ! \
  rtph264depay ! \
  h264parse ! \
  queue ! \
  vaapih264dec ! \
  videoconvert ! \
  'video/x-raw, format=RGBA' ! \
  videoconvert ! \
  fakesink