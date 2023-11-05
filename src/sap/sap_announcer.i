%module sap_announcer

%apply int { uint32_t };  

%include <std_map.i>
%include <std_string.i>
%template(MapUint32String) std::map<uint32_t, std::string>;


%{
#include "sap/sap_announcer.h"
%}

%include "sap/sap_announcer.h"