//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtp_uncompressed_payloader.cc
///
/// Might need to add a route here (hint to send out multicast traffic):
/// sudo route add -net 239.0.0.0 netmask 255.0.0.0 eth1
///

#include <pthread.h>
#include <sched.h>

#include <iostream>
#include <limits>
#include <string>
#include <vector>
#if _WIN32
#include <WS2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#endif

#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_payloader.h"

namespace mediax::rtp::uncompressed {

RtpUncompressedPayloader::RtpUncompressedPayloader() = default;

uint32_t RtpUncompressedPayloader::sequence_number_ = 0;

std::vector<uint8_t> RtpUncompressedPayloader::buffer_in_;

RtpUncompressedPayloader::~RtpUncompressedPayloader(void) {
  if (tx_thread_.joinable()) tx_thread_.join();
  if (GetEgressPort().sockfd) {
    close(GetEgressPort().sockfd);
  }
}

void RtpUncompressedPayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  GetEgressPort().encoding = stream_information.encoding;
  GetEgressPort().height = stream_information.height;
  GetEgressPort().width = stream_information.width;
  GetEgressPort().framerate = stream_information.framerate;
  GetEgressPort().name = stream_information.session_name;
  GetEgressPort().hostname = stream_information.hostname;
  GetEgressPort().port_no = stream_information.port;
  GetEgressPort().settings_valid = true;
  buffer_in_.resize(GetEgressPort().height * GetEgressPort().width * 2);
}

bool RtpUncompressedPayloader::Open() {
  if (!GetEgressPort().port_no) {
    std::cerr << "No ports set, nothing to open";
    exit(-1);
  }

  if (GetEgressPort().port_no) {
    // socket: create the outbound socket
    GetEgressPort().sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (GetEgressPort().sockfd < 0) {
      std::cerr << "ERROR opening socket\n";
      exit(-1);
    }

    // gethostbyname: get the server's DNS entry
    getaddrinfo(GetEgressPort().hostname.c_str(), nullptr, nullptr, &server_out_);
    if (server_out_ == nullptr) {
      std::cerr << "ERROR, no such host as " << GetEgressPort().hostname << "\n";
      exit(-1);
    }

    // build the server's Internet address
    memset(&server_addr_out_, 0, sizeof(server_addr_out_));
    server_addr_out_.sin_family = AF_INET;
    server_addr_out_.sin_addr.s_addr = inet_addr(GetEgressPort().hostname.c_str());
    server_addr_out_.sin_port = htons((uint16_t)GetEgressPort().port_no);

    // send the message to the server
    server_len_out_ = sizeof(server_addr_out_);
    GetEgressPort().socket_open = true;
  }

  return true;
}

void RtpUncompressedPayloader::Close() {
  if (GetEgressPort().port_no) {
    close(GetEgressPort().sockfd);
    GetEgressPort().sockfd = 0;
    GetEgressPort().socket_open = false;
  }
  if (tx_thread_.joinable()) tx_thread_.join();
}

void RtpUncompressedPayloader::UpdateHeader(::mediax::rtp::RtpHeader *packet, int line, int bytes_per_pixel, int last,
                                            int32_t timestamp, int32_t source) {
  memset(reinterpret_cast<std::byte *>(packet), 0, sizeof(::mediax::rtp::RtpHeader));
  packet->rtp.protocol = ::mediax::rtp::kRtpVersion << 30;
  packet->rtp.protocol = packet->rtp.protocol | ::mediax::rtp::kRtpExtension << 28;
  packet->rtp.protocol = packet->rtp.protocol | ::mediax::rtp::kRtpPayloadType << 16;
  packet->rtp.protocol = packet->rtp.protocol | (sequence_number_ & 0xffff);
  packet->rtp.timestamp = timestamp;
  packet->rtp.source = source;
  packet->payload.extended_sequence_number = (sequence_number_ >> 16) & 0xffff;
  packet->payload.line[0].length = static_cast<uint16_t>(GetEgressPort().width * bytes_per_pixel);
  packet->payload.line[0].line_number = (int16_t)line;
  packet->payload.line[0].offset = 0x8000;  // Indicates another line
  packet->payload.line[1].length = 0;
  packet->payload.line[1].line_number = 0;
  packet->payload.line[1].offset = 0x0000;
  if (last == 1) {
    packet->rtp.protocol = packet->rtp.protocol | 1 << 23;
  }
  sequence_number_++;
}

void RtpUncompressedPayloader::SendFrame(RtpUncompressedPayloader *stream) {
  ::mediax::rtp::RtpPacket packet;
  ssize_t n = 0;
  uint32_t timestamp = GenerateTimestamp90kHz();

  if (stream->GetEgressPort().encoding == ::mediax::rtp::ColourspaceType::kColourspaceUndefined) {
    std::cerr << "Colourspace not defined!!\n";
  }
  uint8_t bytes_per_pixel = ::mediax::rtp::kColourspaceBytes.at(stream->GetEgressPort().encoding);
  int32_t stride = stream->GetEgressPort().width * bytes_per_pixel;

  /// Note DEF-STAN 00-082 starts line numbers at 1, gstreamer starts at 0 for raw video
  for (uint32_t c = 1; c <= (stream->GetEgressPort().height); c++) {
    uint32_t last = 0;

    if (c == stream->GetEgressPort().height) last = 1;
    stream->UpdateHeader(reinterpret_cast<::mediax::rtp::RtpHeader *>(&packet), c, bytes_per_pixel, last, timestamp,
                         ::mediax::rtp::kRtpSource);

    EndianSwap32(reinterpret_cast<uint32_t *>(&packet), sizeof(::mediax::rtp::RtpHeaderData) / 4);
    EndianSwap16(reinterpret_cast<uint16_t *>(&packet.head.payload), sizeof(::mediax::rtp::RtpPayloadHeader) / 2);

    memcpy(reinterpret_cast<uint8_t *>(&packet.head.payload.line[2]),
           reinterpret_cast<uint8_t *>(&stream->arg_tx.rgb_frame[c * stride]), stride);
    n = sendto(stream->GetEgressPort().sockfd, &packet, stride + 26, 0, (const sockaddr *)&stream->server_addr_out_,
               stream->server_len_out_);
    if (n != stride + 26) {
      std::cerr << "Transmit socket failure fd=" << stream->GetEgressPort().sockfd << "\n";
    }

    if (n == 0) {
      std::cerr << "Transmit socket failure fd=" << stream->GetEgressPort().sockfd << "\n";
      return;
    }
  }
}

void RtpUncompressedPayloader::TransmitThread(RtpUncompressedPayloader *stream) {
  // send a frame, once last thread has completed
  std::scoped_lock lock(stream->mutex_);
  SendFrame(stream);
  return;
}

int RtpUncompressedPayloader::Transmit(uint8_t *rgbframe, bool blocking) {
  arg_tx.rgb_frame = rgbframe;

  if (GetEgressPort().port_no == 0) return -1;

  if (::mediax::rtp::kRtpThreaded) {
    // Wait for the last thread to finish
    // if (tx_thread_.joinable()) tx_thread_.join();
    // Start a thread so we can start capturing the next frame while transmitting the data
    tx_thread_ = std::thread(TransmitThread, this);
    if (blocking) {
      tx_thread_.join();
    } else {
      tx_thread_.detach();
    }
  } else {
    TransmitThread(this);
  }

  return 0;
}

int32_t RtpUncompressedPayloader::GenerateTimestamp90kHz() {
  // Get the current time point
  auto now = std::chrono::high_resolution_clock::now();

  // Convert the time point to a duration since the epoch
  auto duration = now.time_since_epoch();

  // Convert the duration to a number of 90 kHz units
  int64_t num_90kHz_units =
      std::chrono::duration_cast<std::chrono::duration<int32_t, std::ratio<1, 90000>>>(duration).count();

  // Check that the resulting value fits in a 32-bit signed integer
  if (num_90kHz_units > std::numeric_limits<int32_t>::max() || num_90kHz_units < std::numeric_limits<int32_t>::min()) {
    throw std::overflow_error("Timestamp value out of range for 32-bit signed integer");
  }

  return static_cast<int32_t>(num_90kHz_units);
}

}  // namespace mediax::rtp::uncompressed
