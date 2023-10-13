%module rtp_h264_gst_nvidia_payloader

%import "rtp/rtp_payloader.i"

%{
#include "h264/gst/nvidia/rtp_h264_payloader.h"
%}

%include "h264/gst/nvidia/rtp_h264_payloader.h" 