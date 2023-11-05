%module sap_listener

%apply int { uint32_t };  
// string
%include "std_string.i"
%rename("string") std::string;

%{
#include "sap/sap_listener.h"
%}

%include "std_map.i"

%template(SapAnnouncementsMap) std::map<std::string, ::mediax::sap::SdpMessage, std::less<>>;

%include "sap/sap_listener.h"