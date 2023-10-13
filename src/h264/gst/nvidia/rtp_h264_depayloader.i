%module rtp_h264_gst_nvidia_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "h264/gst/nvidia/rtp_h264_depayloader.h"
%}

%include "h264/gst/nvidia/rtp_h264_depayloader.h" 