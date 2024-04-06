%module rtp_h265_gst_vaapi_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "av1/gst/rtp_av1_depayloader.h"
%}

%include "av1/gst/rtp_av1_depayloader.h" 