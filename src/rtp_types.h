//
// MIT License
//
// Copyright (c) 2023 DefenceX (enquiries@defencex.ai)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the 'Software'), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.ING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
  __attribute__((aligned(16)));
  int16_t line_number;
  __attribute__((aligned(16)));
  int16_t offset;
  __attribute__((aligned(16)));
};

struct PayloadHeader {
  int16_t extended_sequence_number;
  __attribute__((aligned(16)));
  LineHeader line[kNumberLinesPerPacket];  // TODO (ross@rossnewman.com): This can be multiline.
};

struct Header {
  RtpHeader rtp;
  PayloadHeader payload;
};

struct RtpPacket {
  Header head;
  __attribute__((aligned(32)));
  int8_t data[kMaximumBufferSize];
  __attribute__((aligned(8)));
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