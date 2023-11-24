%module rtp_types

%apply int { uint8_t };  
%apply int { uint16_t };  
%apply int { uint32_t };  

%include "std_array.i"

// String
%include "std_string.i"
%rename("string") std::string;

%{
#include "rtp/rtp_types.h"
%}

%include "rtp/rtp_types.h" 