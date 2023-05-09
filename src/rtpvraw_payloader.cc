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
///	sudo route add -net 239.0.0.0 netmask 255.0.0.0 eth1
///

#include <pthread.h>
#include <sched.h>

#include <iostream>
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

#include "rtp_types.h"
#include "rtp_utils.h"
#include "rtpvraw_payloader.h"
#include "sap_announcer.h"

uint32_t RtpvrawPayloader::sequence_number_ = 0;

std::vector<uint8_t> RtpvrawPayloader::buffer_in_;

RtpvrawPayloader::RtpvrawPayloader() { pthread_mutex_init(&mutex_, nullptr); }

RtpvrawPayloader::~RtpvrawPayloader(void) = default;

void RtpvrawPayloader::SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                                     std::string_view hostname, const uint32_t portno) {
  egress_.encoding = encoding;
  egress_.height = height;
  egress_.width = width;
  egress_.framerate = 25;
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
      fprintf(stderr, "ERROR, no such host as %s\n", egress_.hostname.c_str());
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

    // Lastly start a SAP announcement
    sap::SAPAnnouncer::GetInstance().AddSAPAnnouncement(
        {egress_.name, egress_.hostname, egress_.port_no, egress_.height, egress_.width, egress_.framerate, false});
    sap::SAPAnnouncer::GetInstance().Start();
  }

  return true;
}

void RtpvrawPayloader::Close() {
  if (egress_.port_no) {
    close(egress_.sockfd);
    egress_.sockfd = 0;
    egress_.socket_open = false;
  }
}

void RtpvrawPayloader::UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source) const {
  memset((char *)packet, 0, sizeof(Header));
  packet->rtp.protocol = kRtpVersion << 30;
  packet->rtp.protocol = packet->rtp.protocol | kRtpExtension << 28;
  packet->rtp.protocol = packet->rtp.protocol | kRtpPayloadType << 16;
  packet->rtp.protocol = packet->rtp.protocol | (sequence_number_ & 0xffff);
  packet->rtp.timestamp = timestamp;
  packet->rtp.source = source;
  packet->payload.extended_sequence_number = (sequence_number_ >> 16) & 0xffff;
  packet->payload.line[0].length = (int16_t)egress_.width * 2;
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

void RtpvrawPayloader::TransmitThread(RtpvrawPayloader *stream) {
  RtpPacket packet;

  ssize_t n = 0;
  uint32_t timestamp = GenerateTimestamp90kHz();

  int32_t stride = stream->egress_.width * 2;

  // send a frame, once last thread has completed
  pthread_mutex_lock(&stream->mutex_);
  {
    for (uint32_t c = 0; c < (stream->egress_.height); c++) {
      uint32_t last = 0;

      if (c == stream->egress_.height - 1) last = 1;
      stream->UpdateHeader((Header *)&packet, c, last, timestamp, kRtpSource);

      EndianSwap32((uint32_t *)(&packet), sizeof(RtpHeader) / 4);
      EndianSwap16((uint16_t *)(&packet.head.payload), sizeof(PayloadHeader) / 2);

      memcpy((void *)&packet.head.payload.line[2], (void *)&stream->arg_tx.rgbframe[(c * stride) + 1], stride);
      n = sendto(stream->egress_.sockfd, (uint8_t *)&packet, stride + 26, 0,
                 (const sockaddr *)&stream->server_addr_out_, stream->server_len_out_);

      if (n == 0) {
        std::cerr << "[RTP] Transmit socket failure fd=" << stream->egress_.sockfd << "\n";
        return;
      }
    }
  }
  pthread_mutex_unlock(&stream->mutex_);
  return;
}

int RtpvrawPayloader::Transmit(uint8_t *rgbframe, bool blocking) {
  arg_tx.rgbframe = rgbframe;

  if (egress_.port_no == 0) return -1;

  if (kRtpThreaded) {
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

ColourspaceType RtpvrawPayloader::GetColourSpace() const { return egress_.encoding; }

uint32_t RtpvrawPayloader::GetHeight() const { return egress_.height; }

uint32_t RtpvrawPayloader::GetWidth() const { return egress_.width; }

uint32_t RtpvrawPayloader::GetFrameRate() const { return egress_.framerate; }

std::string RtpvrawPayloader::GetIpAddress() const { return egress_.hostname; }

uint32_t RtpvrawPayloader::GetPort() const { return egress_.port_no; }
