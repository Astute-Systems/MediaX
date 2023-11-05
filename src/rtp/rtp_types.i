%module rtp_types

%apply int { uint8_t };  
%apply int { uint16_t };  
%apply int { uint32_t };  
%apply int { int8_t };
%apply int { int16_t };
%apply int { int32_t };

%include "std_array.i"
%array_class(uint8_t, uint8_tArray);
%array_class(uint16_t, uint16_tArray);
%array_class(uint32_t, uint32_tArray);
%array_class(int8_t, int8_tArray);
%array_class(int16_t, int16_tArray);
%array_class(int32_t, int32_tArray);


%{
#include "rtp/rtp_types.h"
%}

%include "rtp/rtp_types.h" 