%module rtp_h264_gst_open_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "h264/gst/open/rtp_h264_depayloader.h"
%}

%include "h264/gst/open/rtp_h264_depayloader.h" 