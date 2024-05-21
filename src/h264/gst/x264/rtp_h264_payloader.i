%module rtp_h264_gst_x264_payloader

%import "rtp/rtp_payloader.i"

%{
#include "h264/gst/x264/rtp_h264_payloader.h"
%}

%include "h264/gst/x264/rtp_h264_payloader.h" 