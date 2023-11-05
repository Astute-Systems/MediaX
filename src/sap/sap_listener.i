%module sap_listener

%apply int { uint32_t };  

%{
#include "sap/sap_listener.h"
%}

%include "std_map.i"
%include "std_string.i"

%template(SapAnnouncementsMap) std::map<std::string, ::mediax::sap::SdpMessage, std::less<>>;

%include "sap/sap_listener.h"