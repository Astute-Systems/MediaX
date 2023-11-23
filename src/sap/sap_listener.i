%module sap_listener

%{
#include "sap/sap_listener.h"
%}

%include "std_map.i"

%include "std_string.i"

%template(SapAnnouncementsMap) std::map<std::string, ::mediax::sap::SdpMessage, std::less<>>;

%apply int { uint32_t };  

%rename("string") std::string;

%include "sap/sap_listener.h"