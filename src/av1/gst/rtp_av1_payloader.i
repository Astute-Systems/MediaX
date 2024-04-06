%module rtp_av1_gst_payloader

%import "rtp/rtp_payloader.i"

%{
#include "av1/gst/rtp_av1_payloader.h"
%}

%include "av1/gst/rtp_av1_payloader.h" 