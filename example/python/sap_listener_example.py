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
SapListener = sap_listener.SapListener.GetInstance()

SapListener.Start()

print("Waiting for 2 seconds for all SAP/SDP announcements")

# Sleep 2 seconds
time.sleep(2)

SapListener.Stop()

announcements = SapListener.GetSapAnnouncements()

# Dump all discovered announcements.
if not announcements:
    print("No SAP/SDP announcements seen")
    sys.exit(0)

print ("SAP/SDP announcements seen:")
for name, sdp in announcements.items():
    from datetime import datetime
    print(datetime.now().strftime("%Y-%m-%d %H:%M:%S") + ":", end="")

    print(f":{sdp.session_name}:{sdp.ip_address_source}:{sdp.ip_address}:{sdp.port}:{sdp.height}:{sdp.width}:{sdp.framerate}:{sdp.sampling}")
    ## sdp.sdp insert /t after /n to make it more readable
    sdp.sdp_text = sdp.sdp_text.replace("\n", "\n\t")
    print(f"\t{sdp.sdp_text}")