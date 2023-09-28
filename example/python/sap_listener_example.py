#!/usr/bin/python3 
#
# Copyright (c) 2023, DefenceX PTY LTD
#
# This file is part of the VivoeX project developed by DefenceX.
#
# Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
# License. See the LICENSE file in the project root for full license details.
#

import time
import sys
sys.path.append('/usr/local/lib/python3/dist-packages/mediax')

import sap_listener
SAPListener = sap_listener.SAPListener.GetInstance()

SAPListener.Start()

print("Waiting for 2 seconds for all SAP/SDP announcements")

# Sleep 2 seconds
time.sleep(2)

SAPListener.Stop()

announcements = SAPListener.GetSAPAnnouncements()

# Dump all discovered announcements.
if not announcements:
    print("No SAP/SDP announcements seen")
    sys.exit(0)

for name, sdp in announcements.items():
     print(f"SDP> name: {sdp.session_name}, source: {sdp.ip_address_source}, ipaddr: {sdp.ip_address}:{sdp.port}, height: {sdp.height}, width: {sdp.width}, framerate: {sdp.framerate}, sampling: {sdp.sampling}")