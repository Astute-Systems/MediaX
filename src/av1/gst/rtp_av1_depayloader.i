%module rtp_av1_gst_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "av1/gst/rtp_av1_depayloader.h"
%}

%include "av1/gst/rtp_av1_depayloader.h" 