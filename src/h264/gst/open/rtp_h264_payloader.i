%module rtp_h264_gst_open_payloader

%import "rtp/rtp_payloader.i"

%{
#include "h264/gst/open/rtp_h264_payloader.h"
%}

%include "h264/gst/open/rtp_h264_payloader.h" 