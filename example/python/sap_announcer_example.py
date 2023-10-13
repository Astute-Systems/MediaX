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

import rtp_types
import sap_announcer

sap = sap_announcer.SAPAnnouncer.GetInstance()
type = rtp_types.ColourspaceType_kColourspaceYuv

# Choose a source interface
# sap.ListInterfaces()

message = sap_announcer.SAPMessage
message.sessionName = "Stream 1"
message.ipAddress = "192.168.1.1"
message.port = 5000
message.height = 480
message.width = 640
message.framerate = 25
message.colourspace = rtp_types.ColourspaceType_kColourspaceYuv
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 2"
message.ipAddress = "192.168.1.2"
message.port = 6000
message.height = 800
message.width = 600
message.framerate = 30
message.colourspace = rtp_types.ColourspaceType_kColourspaceYuv
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 3"
message.ipAddress = "192.168.1.3"
message.port = 7000
message.height = 1920
message.width = 1024
message.framerate = 60
message.colourspace = rtp_types.ColourspaceType_kColourspaceYuv
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 4"
message.ipAddress = "192.168.1.4"
message.port = 8000
message.height = 1920
message.width = 1024
message.framerate = 60
message.colourspace = rtp_types.ColourspaceType_kColourspaceRgb24
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 5"
message.ipAddress = "192.168.1.5"
message.port = 9000
message.height = 1920
message.width = 1024
message.framerate = 60
message.colourspace = rtp_types.ColourspaceType_kColourspaceMono8
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 6"
message.ipAddress = "192.168.1.6"
message.port = 10000
message.height = 1920
message.width = 1024
message.framerate = 60
message.colourspace = rtp_types.ColourspaceType_kColourspaceJpeg2000
sap.AddSapAnnouncement(message())

message.sessionName = "Stream 7"
message.ipAddress = "192.168.1.7"
message.port = 20000
message.height = 1920
message.width = 1024
message.framerate = 60
message.colourspace = rtp_types.ColourspaceType_kColourspaceH264Part10
sap.AddSapAnnouncement(message())

sap.Start()

# Sleep 10 seconds
time.sleep(10)

sap.Stop()
