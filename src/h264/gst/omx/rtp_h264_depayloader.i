%module rtp_h264_gst_omx_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "h264/gst/omx/rtp_h264_depayloader.h"
%}

%include "h264/gst/omx/rtp_h264_depayloader.h" 