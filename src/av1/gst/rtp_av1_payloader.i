%module rtp_h265_gst_vaapi_depayloader

%import "rtp/rtp_payloader.i"

%{
#include "h265/gst/vaapi/rtp_av1_payloader.h"
%}

%include "h265/gst/vaapi/rtp_av1_payloader.h" 