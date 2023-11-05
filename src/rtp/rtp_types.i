%module rtp_types

%apply int { uint8_t };  
%apply int { uint16_t };  
%apply int { uint32_t };  

%include "std_array.i"
%array_class(uint8_t, uint8_tArray);
%array_class(uint16_t, uint16_tArray);
%array_class(uint32_t, uint32_tArray);

// String
%include "std_string.i"
%rename("string") std::string;

%{
#include "rtp/rtp_types.h"
%}

%include "rtp/rtp_types.h" 