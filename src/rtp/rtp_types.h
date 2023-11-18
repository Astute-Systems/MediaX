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

#ifndef RTP_RTP_TYPES_H_
#define RTP_RTP_TYPES_H_

#include <stdint.h>

#include <array>
#include <map>
#include <string>

namespace mediax::rtp {

enum class StatusCode { kStatusOk = 0, kStatusError };

/// SAP/SDP port
const uint16_t kSapPort = 9875;

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
const uint32_t kMaxUdpData = kMaximumBufferSize + 100;
/// 0 to disable RTP header checking
const uint32_t kRtpCheck = 0;
/// transmit and recieve in a thread. RX thread blocks TX does not
const uint32_t kRtpThreaded = 1;
/// RGBX processing kPitch
const uint32_t kPitch = 4;

/// 4 byte float
struct float4 {
  /// x position
  float x;
  /// y position
  float y;
  /// z position
  float z;
  /// w position
  float w;
};

/// 12 byte RTP Raw video header
struct RtpHeaderData {
  /// RTP version
  uint32_t protocol : 32;
  /// sent timestamp
  uint32_t timestamp : 32;
  /// source identifier
  uint32_t source : 32;
};

/// Line header
struct RtpLineHeader {
  /// Length of line in bytes
  uint16_t length;
  /// Line number
  uint16_t line_number;
  /// Offset in bytes from start of line
  uint16_t offset;
};

/// Supported colour spaces
enum class ColourspaceType {
  kColourspaceRgb24 = 0,
  kColourspaceYuv,
  kColourspaceMono8,
  kColourspaceMono16,
  kColourspaceRgba,  // Not a GVA uncompressed video format
  kColourspaceBgra,  // Not a GVA uncompressed video format
  kColourspaceNv12,  // Not a GVA uncompressed video format
  kColourspaceJpeg2000,
  kColourspaceH264Part4,
  kColourspaceH264Part10,
  kColourspaceH265,
  kColourspaceUndefined
};

/// The bits per pixel
const std::map<ColourspaceType, uint8_t> kColourspaceBytes = {
    {ColourspaceType::kColourspaceUndefined, 0}, {ColourspaceType::kColourspaceRgb24, 3},
    {ColourspaceType::kColourspaceYuv, 2},       {ColourspaceType::kColourspaceMono8, 1},
    {ColourspaceType::kColourspaceMono16, 2},    {ColourspaceType::kColourspaceJpeg2000, 3},
    {ColourspaceType::kColourspaceH264Part4, 3}, {ColourspaceType::kColourspaceH264Part10, 3}};

/// SDP encoding type
const std::map<ColourspaceType, std::string> kRtpMap = {
    {ColourspaceType::kColourspaceUndefined, "unknown"}, {ColourspaceType::kColourspaceRgb24, "raw"},
    {ColourspaceType::kColourspaceYuv, "raw"},           {ColourspaceType::kColourspaceMono8, "raw"},
    {ColourspaceType::kColourspaceMono16, "raw"},        {ColourspaceType::kColourspaceJpeg2000, "jpeg2000"},
    {ColourspaceType::kColourspaceH264Part4, "nv12"},    {ColourspaceType::kColourspaceH264Part10, "nv12"}};
/// SDP colourspace if applicable
const std::map<ColourspaceType, std::string> kColourspace = {
    {ColourspaceType::kColourspaceUndefined, "unknown"},
    {ColourspaceType::kColourspaceRgb24, "RGB"},
    {ColourspaceType::kColourspaceYuv, "YCbCr-4:2:2"},
    {ColourspaceType::kColourspaceMono8, "GRAYSCALE"},
    {ColourspaceType::kColourspaceMono16, "GRAYSCALE"},
    {ColourspaceType::kColourspaceJpeg2000, "YCbCr-4:2:2"},
    {ColourspaceType::kColourspaceH264Part4, "H264 (Part 4)"},
    {ColourspaceType::kColourspaceH264Part10, "H264 (Part 10)"}};

/// Store common port information for ingress and egress ports
struct RtpPortType {
  /// Hostname or IP address
  std::string hostname;
  /// Port number
  uint32_t port_no = 0;
  /// Socket file descriptor
  int sockfd = 0;
  /// The session name
  std::string name = "unknown";
  /// Height in pixels of stream
  uint32_t height = 0;
  /// Width in pixels of stream
  uint32_t width = 0;
  /// Intended update framerate
  uint32_t framerate = 25;
  /// Encoding type
  ColourspaceType encoding;
  /// Socket open flag
  bool socket_open = false;
  /// Can be set when SAP/SDP arrives or gets deleted
  bool settings_valid = false;
};

/// RTP Payload header
struct RtpPayloadHeader {
  /// Extended sequence number
  uint16_t extended_sequence_number;
  /// Line header/s
  std::array<RtpLineHeader, kNumberLinesPerPacket> line;  // This can be multiline min the future
};

/// RTP packet structure
struct RtpHeader {
  /// RTP header
  RtpHeaderData rtp;
  /// RTP payload header
  RtpPayloadHeader payload;
};

/// RTP packet structure
struct RtpPacket {
  /// RTP header
  RtpHeader head;
  /// RTP payload
  std::array<int8_t, kMaximumBufferSize> data;
};

/// Resolution of a video stream
struct Resolution {
  /// The height of the video stream
  uint32_t height;
  /// The width of the video stream
  uint32_t width;
};

/// The RTP callback data
struct RtpCallbackData {
  Resolution resolution;
  uint8_t *cpu_buffer;
  mediax::rtp::ColourspaceType encoding;
};

/// Transmit data structure
struct TxData {
  /// RGB frame
  uint8_t *rgb_frame;
  /// Encoded frame See ColourspaceType
  uint8_t *encoded_frame;
  /// Height of frame
  uint32_t width;
  /// Width of frame
  uint32_t height;
};

/// Struct capturing all stream information
struct StreamInformation {
  /// The SDP session name
  std::string session_name;
  /// The IPV4 address as a string
  std::string hostname;
  /// The IPV4 port number
  uint32_t port = 5004;
  /// The stream height in pixels
  uint32_t height;
  /// The stream width in pixels
  uint32_t width;
  /// The stream framerate in frames / second
  uint32_t framerate;
  /// Colourspace
  ::mediax::rtp::ColourspaceType encoding;
  /// Flag indicating the stream was deleted
  bool deleted = false;
};

}  // namespace mediax::rtp

#endif  // RTP_RTP_TYPES_H_
