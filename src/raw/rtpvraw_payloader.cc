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
/// \file rtpvraw_payloader.cc
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

#include <glog/logging.h>

#include "raw/rtpvraw_payloader.h"
#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"

namespace mediax {

uint32_t RtpvrawPayloader::sequence_number_ = 0;

std::vector<uint8_t> RtpvrawPayloader::buffer_in_;

RtpvrawPayloader::RtpvrawPayloader() = default;

RtpvrawPayloader::~RtpvrawPayloader(void) {
  if (tx_thread_.joinable()) tx_thread_.join();
  if (egress_.sockfd) {
    close(egress_.sockfd);
  }
}

void RtpvrawPayloader::SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                                     uint32_t framerate, std::string_view hostname, const uint32_t portno) {
  egress_.encoding = encoding;
  egress_.height = height;
  egress_.width = width;
  egress_.framerate = framerate;
  egress_.name = name;
  egress_.hostname = hostname;
  egress_.port_no = portno;
  egress_.settings_valid = true;
  buffer_in_.resize(height * width * 2);
}

bool RtpvrawPayloader::Open() {
  if (!egress_.port_no) {
    std::cerr << "No ports set, nothing to open";
    exit(-1);
  }

  if (egress_.port_no) {
    // socket: create the outbound socket
    egress_.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (egress_.sockfd < 0) {
      std::cerr << "ERROR opening socket\n";
      exit(-1);
    }

    // gethostbyname: get the server's DNS entry
    getaddrinfo(egress_.hostname.c_str(), nullptr, nullptr, &server_out_);
    if (server_out_ == nullptr) {
      std::cerr << "ERROR, no such host as " << egress_.hostname << "\n";
      exit(-1);
    }

    // build the server's Internet address
    memset(&server_addr_out_, 0, sizeof(server_addr_out_));
    server_addr_out_.sin_family = AF_INET;
    server_addr_out_.sin_addr.s_addr = inet_addr(egress_.hostname.c_str());
    server_addr_out_.sin_port = htons((uint16_t)egress_.port_no);

    // send the message to the server
    server_len_out_ = sizeof(server_addr_out_);
    egress_.socket_open = true;
  }

  return true;
}

void RtpvrawPayloader::Close() {
  if (egress_.port_no) {
    close(egress_.sockfd);
    egress_.sockfd = 0;
    egress_.socket_open = false;
  }
  if (tx_thread_.joinable()) tx_thread_.join();
}

void RtpvrawPayloader::UpdateHeader(Header *packet, int line, int bytes_per_pixel, int last, int32_t timestamp,
                                    int32_t source) const {
  memset(reinterpret_cast<char *>(packet), 0, sizeof(Header));
  packet->rtp.protocol = kRtpVersion << 30;
  packet->rtp.protocol = packet->rtp.protocol | kRtpExtension << 28;
  packet->rtp.protocol = packet->rtp.protocol | kRtpPayloadType << 16;
  packet->rtp.protocol = packet->rtp.protocol | (sequence_number_ & 0xffff);
  packet->rtp.timestamp = timestamp;
  packet->rtp.source = source;
  packet->payload.extended_sequence_number = (sequence_number_ >> 16) & 0xffff;
  packet->payload.line[0].length = (int16_t)egress_.width * bytes_per_pixel;
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

void RtpvrawPayloader::SendFrame(RtpvrawPayloader *stream) {
  RtpPacket packet;

  ssize_t n = 0;
  uint32_t timestamp = GenerateTimestamp90kHz();

  if (stream->egress_.encoding == ColourspaceType::kColourspaceUndefined) {
    std::cerr << "Colourspace not defined!!\n";
  }
  uint8_t bytes_per_pixel = kColourspaceBytes.at(stream->egress_.encoding);
  int32_t stride = stream->egress_.width * bytes_per_pixel;

  /// Note DEF-STAN 00-082 starts line numbers at 1, gstreamer starts at 0 for raw video
  for (uint32_t c = 1; c <= (stream->egress_.height); c++) {
    uint32_t last = 0;

    if (c == stream->egress_.height) last = 1;
    stream->UpdateHeader(reinterpret_cast<Header *>(&packet), c, bytes_per_pixel, last, timestamp, kRtpSource);

    EndianSwap32(reinterpret_cast<uint32_t *>(&packet), sizeof(RtpHeader) / 4);
    EndianSwap16(reinterpret_cast<uint16_t *>(&packet.head.payload), sizeof(PayloadHeader) / 2);

    memcpy(reinterpret_cast<void *>(&packet.head.payload.line[2]),
           reinterpret_cast<void *>(&stream->arg_tx.rgb_frame[(c * stride)]), stride);
    n = sendto(stream->egress_.sockfd, &packet, stride + 26, 0, (const sockaddr *)&stream->server_addr_out_,
               stream->server_len_out_);
    if (n != stride + 26) {
      LOG(ERROR) << "Transmit socket failure fd=" << stream->egress_.sockfd;
    }

    if (n == 0) {
      LOG(ERROR) << "Transmit socket failure fd=" << stream->egress_.sockfd;
      return;
    }
  }
}

void RtpvrawPayloader::TransmitThread(RtpvrawPayloader *stream) {
  // send a frame, once last thread has completed
  std::scoped_lock lock(stream->mutex_);
  SendFrame(stream);
  return;
}

int RtpvrawPayloader::Transmit(uint8_t *rgbframe, bool blocking) {
  arg_tx.rgb_frame = rgbframe;

  if (egress_.port_no == 0) return -1;

  if (kRtpThreaded) {
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

int32_t RtpvrawPayloader::GenerateTimestamp90kHz() {
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

}  // namespace mediax
