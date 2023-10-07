%module rtp_uncompressed_payloader

%import "rtp/rtp_payloader.i"

%{
#include "uncompressed/rtp_uncompressed_payloader.h"
%}

%include "uncompressed/rtp_uncompressed_payloader.h"