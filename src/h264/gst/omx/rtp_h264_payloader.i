%module rtp_h264_gst_omx_payloader

%import "rtp/rtp_payloader.i"

%{
#include "h264/gst/omx/rtp_h264_payloader.h"
%}

%include "h264/gst/omx/rtp_h264_payloader.h" 