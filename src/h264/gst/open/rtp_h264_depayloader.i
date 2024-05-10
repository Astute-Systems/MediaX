%module rtp_h264_gst_vaapi_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "h264/gst/vaapi/rtp_h264_depayloader.h"
%}

%include "h264/gst/vaapi/rtp_h264_depayloader.h" 