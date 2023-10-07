%module rtp_uncompressed_depayloader

%import "rtp/rtp_depayloader.i"

%{
#include "uncompressed/rtp_uncompressed_depayloader.h"
%}

%include "uncompressed/rtp_uncompressed_depayloader.h" 