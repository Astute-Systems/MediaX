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
/// \file rtp_uncompressed_depayloader.cc
///
/// Might need to add a route here (hint to send out multicast traffic):
/// sudo route add -net 239.0.0.0 netmask 255.0.0.0 eth1
///

#include <pthread.h>
#include <sched.h>

#include <iostream>
#include <string>
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

#include <stdexcept>
#include <vector>

#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_depayloader.h"

namespace mediax::rtp::uncompressed {

std::vector<uint8_t> RtpUncompressedDepayloader::buffer_in_;

RtpUncompressedDepayloader::RtpUncompressedDepayloader() { pthread_mutex_init(&mutex_, nullptr); }

RtpUncompressedDepayloader::~RtpUncompressedDepayloader(void) = default;

RtpUncompressedDepayloader::RtpUncompressedDepayloader(const RtpUncompressedDepayloader &other) {
  *this = other;
  pthread_mutex_init(&mutex_, nullptr);
}

RtpUncompressedDepayloader &RtpUncompressedDepayloader::operator=(const RtpUncompressedDepayloader &other
                                                                  [[maybe_unused]]) {
  return *this;
}

// Broadcast the stream to port i.e. 5004
void RtpUncompressedDepayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  ingress_.encoding = stream_information.encoding;
  ingress_.height = stream_information.height;
  ingress_.width = stream_information.width;
  ingress_.name = stream_information.session_name;
  ingress_.framerate = stream_information.framerate;
  ingress_.hostname = stream_information.hostname;
  ingress_.port_no = stream_information.port;
  ingress_.settings_valid = true;
  buffer_in_.resize((ingress_.height * ingress_.width) * ::mediax::rtp::kColourspaceBytes.at(ingress_.encoding));
}

bool RtpUncompressedDepayloader::Open() {
  if (!ingress_.port_no) {
    LOG(ERROR) << "RtpUncompressedDepayloader::Open() No ports set, nothing to open";
    exit(-1);
  }
  if (ingress_.port_no) {
    struct sockaddr_in addr;

    // create a UDP socket
    if ((ingress_.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      LOG(ERROR) << "RtpUncompressedDepayloader::Open() ERROR opening socket " << ingress_.hostname << ":"
                 << ingress_.port_no;
      exit(-1);
    }

    if (IsMulticast(ingress_.hostname)) {
      // Join multicast group
      struct ip_mreq mreq;
      mreq.imr_multiaddr.s_addr = inet_addr(ingress_.hostname.c_str());
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      if (setsockopt(ingress_.sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        LOG(ERROR) << "Can not join multicast group " << ingress_.hostname << ":" << ingress_.port_no;
        exit(-1);
      }
      LOG(INFO) << "Joining multicast group " << ingress_.hostname << ":" << ingress_.port_no;
    }

    // zero out the structure
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ingress_.hostname.c_str());
    addr.sin_port = htons((uint16_t)ingress_.port_no);
    // bind socket to port
    if (bind(ingress_.sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      LOG(ERROR) << "RtpUncompressedDepayloader::Open() ERROR binding socket " << ingress_.hostname << ":"
                 << ingress_.port_no;
      exit(-1);
    }
    ingress_.socket_open = true;
  }

  return true;
}

void RtpUncompressedDepayloader::Close() {
  if (ingress_.port_no) {
    close(ingress_.sockfd);
    ingress_.sockfd = 0;
    ingress_.socket_open = false;
  }
}

bool RtpUncompressedDepayloader::ReadRtpHeader(RtpUncompressedDepayloader *stream,
                                               ::mediax::rtp::RtpPacket *packet) const {
  int version;
  int payloadType;

  //
  // Read in the RTP data
  //
  if (ssize_t bytes = recvfrom(RtpDepayloader::ingress_.sockfd, stream->udpdata.data(), ::mediax::rtp::kMaxUdpData, 0,
                               nullptr, nullptr);
      bytes <= 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return false;
  }
  packet = reinterpret_cast<::mediax::rtp::RtpPacket *>(stream->udpdata.data());
  EndianSwap32(reinterpret_cast<uint32_t *>(packet), sizeof(::mediax::rtp::RtpHeaderData) / 4);

  //
  // Decode Header bits and confirm RTP packet
  //
  payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
  version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
  if ((payloadType == 96) && (version == 2)) {
    return true;
  }
  return false;
}

void RtpUncompressedDepayloader::ReceiveThread(RtpUncompressedDepayloader *stream) {
  ::mediax::rtp::RtpPacket *packet{};
  bool receiving = true;
  int scan_count = 0;
  int last_packet;

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(stream->ingress_.sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  while (stream->rx_thread_running_) {
    bool last_scan_line = false;
    while (receiving && stream->rx_thread_running_) {
      int marker;
      int version;
      int payloadType;
      bool valid = false;
      last_scan_line = false;

      //
      // Read data until we get the next RTP header
      //
      while (!valid && stream->rx_thread_running_) {
        //
        // Read in the RTP data
        //

        if (ssize_t bytes = recvfrom(stream->ingress_.sockfd, stream->udpdata.data(), ::mediax::rtp::kMaxUdpData, 0,
                                     nullptr, nullptr);
            bytes <= 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(2));
          continue;
        }
        packet = reinterpret_cast<::mediax::rtp::RtpPacket *>(stream->udpdata.data());
        EndianSwap32(reinterpret_cast<uint32_t *>(packet), sizeof(::mediax::rtp::RtpHeaderData) / 4);

        //
        // Decode Header bits and confirm RTP packet
        //
        payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
        version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
        last_scan_line = (packet->head.rtp.protocol & 0x00800000) >> 23;
        if ((payloadType == 96) && (version == 2)) {
          valid = true;
        }
      }
      if (valid) {
        // Valid packet so start to decode packet
        bool scan_line = true;

        // Decode Header bits
        marker = (packet->head.rtp.protocol & 0x00800000) >> 23;

        //
        // Count the number of scan_lines in the packet
        //
        while (scan_line) {
          int more;
          EndianSwap16(reinterpret_cast<uint16_t *>(&packet->head.payload.line[scan_count]),
                       sizeof(::mediax::rtp::RtpLineHeader) / 2);
          more = (packet->head.payload.line[scan_count].offset & 0x8000) >> 15;
          !more ? scan_line = false : scan_line = true;  // The last scan_line
          scan_count++;
        }

        //
        // Now we know the number of scan_lines we can copy the data
        //
        int payload_offset =
            sizeof(::mediax::rtp::RtpHeaderData) + 2 + (scan_count * sizeof(::mediax::rtp::RtpLineHeader));
        int payload = 0;

        last_packet = payload_offset;
        for (int c = 0; c < scan_count; c++) {
          uint32_t os;
          uint32_t pixel;
          uint32_t length;

          os = payload_offset + payload;
          if (packet->head.payload.line[c].line_number == 0) {
            // Line numbers start at 1 in DEF-STAN 00-82, gstreamer starts at zero so drop those lines
            break;
          }
          pixel = ((packet->head.payload.line[c].offset & 0x7FFF) *
                   ::mediax::rtp::kColourspaceBytes.at(stream->ingress_.encoding)) +
                  (((packet->head.payload.line[c].line_number - 1) & 0x7FFF) *
                   (stream->ingress_.width * ::mediax::rtp::kColourspaceBytes.at(stream->ingress_.encoding)));
          length = packet->head.payload.line[c].length & 0xFFFF;

          memcpy(&RtpUncompressedDepayloader::buffer_in_[pixel], &stream->udpdata[os], length);

          last_packet += length;
          payload += length;
        }

        marker ? receiving = false : receiving = true;

        scan_count = 0;
      }
    }

    stream->arg_tx.encoded_frame = RtpUncompressedDepayloader::buffer_in_.data();
    stream->new_rx_frame_ = last_scan_line;
    receiving = true;
  }  // Receive loop
  return;
}

void RtpUncompressedDepayloader::Start() {
  rx_thread_running_ = true;
  rx_thread_ = std::thread(&RtpUncompressedDepayloader::ReceiveThread, this);
}

void RtpUncompressedDepayloader::Stop() {
  rx_thread_running_ = false;

  if (rx_thread_.joinable()) {
    rx_thread_.join();
  }
}

bool RtpUncompressedDepayloader::WaitForFrame(uint8_t **cpu, int32_t timeout) {
  // Wait for completion
  if (timeout <= 0) {
    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  } else {
    auto to = std::chrono::milliseconds(timeout);
    auto start_time = std::chrono::high_resolution_clock::now();

    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      auto end_time = std::chrono::high_resolution_clock::now();
      if (auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
          duration >= to) {
        // Leave the thread to receive the rest of the frame
        *cpu = nullptr;
        return false;
      }
    }
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  }
}

bool RtpUncompressedDepayloader::Receive(uint8_t **cpu, int32_t timeout) {
  if (ingress_.port_no == 0) {
    LOG(ERROR) << "Port number has not been set";
    return false;
  }

  if (ingress_.settings_valid == false) {
    LOG(ERROR) << "IP settings are invalid";
    return false;
  }

  // Check ports open
  if ((ingress_.socket_open == false) && (ingress_.settings_valid == true)) {
    Open();
  }

  // Check if we have a frame ready
  if (new_rx_frame_) {
    // Dont start a new thread if a frame is available just return it
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  } else {
    return WaitForFrame(cpu, timeout);
  }

  // should not ever get here
  LOG(ERROR) << "Something went wrong should not be here";
  return false;
}

}  // namespace mediax::rtp::uncompressed
