//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
///
/// \brief RTP streaming video types
///
/// \file rtp_types.h

#ifndef __RTP_TYPES_H__
#define __RTP_TYPES_H__

#include <stdint.h>

#define ENDIAN_SWAP __amd64__ || __x86_64__  // Perform endian swap, Intel defined by gcc

const uint32_t kGst1Fix = 1;
const uint32_t kRtpVersion = 0x2;  // RFC 1889 Version 2
const uint32_t kRtpPadding = 0x0;
const uint32_t kRtpExtension = 0x0;
const uint32_t kRtpMarker = 0x0;
const uint32_t kRtpPayloadType = 0x60;   // 96 Dynamic Type
const uint32_t kRtpSource = 0x12345678;  // Should be unique
const uint32_t kRtpFramerate = 25;
const uint32_t Hz90 = 90000;
const uint32_t kNumberLinesPerPacket = 10;     // can have more that one line in a packet
const uint32_t kMaximumBufferSize = 1280 * 3;  // allow for RGB data upto 1280 pixels wide
const uint32_t kMaxUdpData = 1500;             // enough space for three lines of UDP data MTU size should be checked
const uint32_t kRtpCheck = 0;                  // 0 to disable RTP header checking
const uint32_t kRtpThreaded = 1;               // transmit and recieve in a thread. RX thread blocks TX does not
const uint32_t kPitch = 4;                     // RGBX processing kPitch

#if ENDIAN_SWAP
void EndianSwap32(uint32_t *data, unsigned int length);
void EndianSwap16(uint16_t *data, unsigned int length);
#endif

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

class RtpStream;
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