%module rtp_h265_gst_vaapi_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "h265/gst/vaapi/rtp_h265_depayloader.h"
%}

%include "h265/gst/vaapi/rtp_h265_depayloader.h" 