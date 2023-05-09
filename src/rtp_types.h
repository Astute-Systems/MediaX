//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video types
///
/// \file rtp_types.h

#ifndef __RTP_TYPES_H__
#define __RTP_TYPES_H__

#include <stdint.h>

/// SAP/SDP constants
const std::string kIpaddr = "224.2.127.254";
/// SAP/SDP port
const uint16_t kPort = 9875;

// RTP constants
/// RFC 1889 Version 2
const uint32_t kRtpVersion = 0x2;
/// RTP Extension bit
const uint32_t kRtpExtension = 0x0;
/// RTP marker bit
const uint32_t kRtpMarker = 0x0;
/// 96 Dynamic Type
const uint32_t kRtpPayloadType = 0x60;
/// Should be unique
const uint32_t kRtpSource = 0x12345678;
/// 25 frames per second
const uint32_t kRtpFramerate = 25;
/// 90KHz clock
const uint32_t Hz90 = 90000;
/// can have more that one line in a packet
const uint32_t kNumberLinesPerPacket = 10;
/// allow for RGB data upto 1280 pixels wide
const uint32_t kMaximumBufferSize = 1280 * 3;
/// enough space for three lines of UDP data MTU size should be checked
const uint32_t kMaxUdpData = 1500;
/// 0 to disable RTP header checking
const uint32_t kRtpCheck = 0;
/// transmit and recieve in a thread. RX thread blocks TX does not
const uint32_t kRtpThreaded = 1;
/// RGBX processing kPitch
const uint32_t kPitch = 4;

void EndianSwap32(uint32_t *data, unsigned int length);
void EndianSwap16(uint16_t *data, unsigned int length);

struct float4 {
  float x;
  float y;
  float z;
  float w;
};

// 12 byte RTP Raw video header
struct RtpHeader {
  int32_t protocol : 32;
  int32_t timestamp : 32;
  int32_t source : 32;
};

#pragma pack(1)

struct LineHeader {
  int16_t length;
  int16_t line_number;
  int16_t offset;
};

/// Supported colour spaces
enum class ColourspaceType { kColourspaceUndefined, kColourspaceRgb24, kColourspaceYuv, kColourspaceMono8 };

/// Store common port information for ingress and egress ports
struct PortType {
  std::string hostname;
  uint32_t port_no = 0;
  int sockfd = 0;
  std::string name = "unknown";
  /// Height in pixels of stream
  uint32_t height = 0;
  /// Width in pixels of stream
  uint32_t width = 0;
  /// Intended update framerate
  uint32_t framerate = 25;
  ColourspaceType encoding;
  bool socket_open = false;
  bool settings_valid = false;  // Can be set when SAP/SDP arrives or gets deleted
};

struct PayloadHeader {
  int16_t extended_sequence_number;
  LineHeader line[kNumberLinesPerPacket];  // This can be multiline min the future
};

struct Header {
  RtpHeader rtp;
  PayloadHeader payload;
};

struct RtpPacket {
  Header head;
  int8_t data[kMaximumBufferSize];
};

//
// Transmit data structure
// Battlefield Management System (BMS) state definition
struct TxData {
  uint8_t *rgbframe;
  uint8_t *yuvframe;
  uint32_t width;
  uint32_t height;
};

#pragma pack()

#endif  // __RTP_TYPES_H__