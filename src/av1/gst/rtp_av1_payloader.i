%module rtp_h265_gst_vaapi_depayloader

%import "rtp/rtp_payloader.i"

%{
#include "av1/gst/rtp_av1_payloader.h"
%}

%include "av1/gst/rtp_av1_payloader.h" 