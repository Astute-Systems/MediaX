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

RtpUncompressedDepayloader::RtpUncompressedDepayloader() { pthread_mutex_init(&mutex_, nullptr); }

// Broadcast the stream to port i.e. 5004
void RtpUncompressedDepayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  GetStream().encoding = stream_information.encoding;
  GetStream().height = stream_information.height;
  GetStream().width = stream_information.width;
  GetStream().name = stream_information.session_name;
  GetStream().framerate = stream_information.framerate;
  GetStream().hostname = stream_information.hostname;
  GetStream().port_no = stream_information.port;
  GetStream().settings_valid = true;
  GetBuffer().resize((GetStream().height * GetStream().width) *
                     ::mediax::rtp::kColourspaceBytes.at(GetStream().encoding));
}

bool RtpUncompressedDepayloader::Open() {
  // Call the base class
  RtpDepayloader::Open();

  if (!GetStream().port_no) {
    LOG(ERROR) << "RtpUncompressedDepayloader::Open() No ports set, nothing to open";
    exit(-1);
  }
  if (GetStream().port_no) {
    struct sockaddr_in addr;

    // create a UDP socket
    if ((GetStream().sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      LOG(ERROR) << "RtpUncompressedDepayloader::Open() ERROR opening socket " << GetStream().hostname << ":"
                 << GetStream().port_no;
      exit(-1);
    }

    if (IsMulticast(GetStream().hostname)) {
      // Join multicast group
      struct ip_mreq mreq;
      mreq.imr_multiaddr.s_addr = inet_addr(GetStream().hostname.c_str());
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      if (setsockopt(GetStream().sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        LOG(ERROR) << "Can not join multicast group " << GetStream().hostname << ":" << GetStream().port_no;
        exit(-1);
      }
    }

    // zero out the structure
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(GetStream().hostname.c_str());
    addr.sin_port = htons((uint16_t)GetStream().port_no);
    // bind socket to port, allow reuse

    if (int opt = 1;
        setsockopt(GetStream().sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&opt), sizeof(opt)) < 0) {
      LOG(ERROR) << "RtpUncompressedDepayloader::Open() ERROR setting socket options " << GetStream().hostname << ":"
                 << GetStream().port_no;
      exit(-1);
    }
    if (bind(GetStream().sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      LOG(ERROR) << "RtpUncompressedDepayloader::Open() ERROR binding socket " << GetStream().hostname << ":"
                 << GetStream().port_no;
      exit(-1);
    }
    GetStream().socket_open = true;
  }

  return true;
}

void RtpUncompressedDepayloader::Close() {
  // Call the base class
  RtpDepayloader::Close();

  // Call the base class
  RtpDepayloader::Close();
  if (GetStream().port_no) {
    close(GetStream().sockfd);
    GetStream().sockfd = 0;
    GetStream().socket_open = false;
  }
}

bool RtpUncompressedDepayloader::ReadRtpHeader(RtpUncompressedDepayloader *stream, ::mediax::rtp::RtpPacket *packet) {
  int version;
  int payloadType;

  //
  // Read in the RTP data
  //
  if (ssize_t bytes =
          recvfrom(GetStream().sockfd, stream->udpdata.data(), ::mediax::rtp::kMaxUdpData, 0, nullptr, nullptr);
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

bool RtpUncompressedDepayloader::ReceiveLines(::mediax::rtp::RtpPacket *packet, bool *last_scan_line,
                                              int *last_packet) {
  bool receiving = true;
  int scan_count = 0;
  int marker;
  int version;
  int payloadType;
  bool valid = false;

  //
  // Read data until we get the next RTP header
  //
  while (!valid && rx_thread_running_) {
    //
    // Read in the RTP data
    //

    if (ssize_t bytes = recvfrom(GetStream().sockfd, udpdata.data(), ::mediax::rtp::kMaxUdpData, 0, nullptr, nullptr);
        bytes <= 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      continue;
    }
    packet = reinterpret_cast<::mediax::rtp::RtpPacket *>(udpdata.data());
    EndianSwap32(reinterpret_cast<uint32_t *>(packet), sizeof(::mediax::rtp::RtpHeaderData) / 4);

    //
    // Decode Header bits and confirm RTP packet
    //
    payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
    version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
    *last_scan_line = (packet->head.rtp.protocol & 0x00800000) >> 23;
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
    int payload_offset = sizeof(::mediax::rtp::RtpHeaderData) + 2 + (scan_count * sizeof(::mediax::rtp::RtpLineHeader));
    int payload = 0;

    *last_packet = payload_offset;
    for (int c = 0; c < scan_count; c++) {
      uint32_t os;
      uint64_t pixel;
      uint32_t length;

      os = payload_offset + payload;
      if (packet->head.payload.line[c].line_number == 0) {
        // Line numbers start at 1 in DEF-STAN 00-82, gstreamer starts at zero so drop those lines
        break;
      }
      pixel =
          ((packet->head.payload.line[c].offset & 0x7FFF) * ::mediax::rtp::kColourspaceBytes.at(GetStream().encoding)) +
          (((packet->head.payload.line[c].line_number - 1) & 0x7FFF) *
           (GetStream().width * ::mediax::rtp::kColourspaceBytes.at(GetStream().encoding)));

      length = packet->head.payload.line[c].length & 0xFFFF;
      memcpy(&GetBuffer().data()[pixel], &udpdata[os], length);

      last_packet += length;
      payload += length;
    }

    marker ? receiving = false : receiving = true;
    if (marker) {
      uint16_t sequence_number_low = packet->head.rtp.protocol & 0xffff;
      uint16_t sequence_number_hight = packet->head.payload.extended_sequence_number;
      EndianSwap16(&sequence_number_hight, sizeof(uint16_t));
      uint32_t new_sequence_number =
          (((sequence_number_hight & 0xffff) << 16) | sequence_number_low) / GetStream().height;
      sequence_number_ = new_sequence_number;
    }
  }
  return receiving;
}

void RtpUncompressedDepayloader::ReceiveThread(RtpUncompressedDepayloader *stream) {
  ::mediax::rtp::RtpPacket *packet{};
  int last_packet;

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(stream->GetStream().sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  while (stream->rx_thread_running_) {
    bool last_scan_line = false;
    stream->ReceiveLines(packet, &last_scan_line, &last_packet);
    // Have a complete frame now.
    stream->arg_tx.encoded_frame = stream->GetBuffer().data();
    stream->new_rx_frame_ = last_scan_line;

    if ((stream->CallbackRegistered()) && stream->new_rx_frame_) {
      ::mediax::rtp::RtpFrameData arg_tx = {
          {stream->GetHeight(), stream->GetWidth()}, stream->GetBuffer().data(), stream->GetColourSpace()};
      stream->Callback(arg_tx);
    }
  }  // Receive loop

  return;
}

void RtpUncompressedDepayloader::Start() {
  // Call the base class
  RtpDepayloader::Start();
  rx_thread_running_ = true;
  rx_thread_ = std::thread(&RtpUncompressedDepayloader::ReceiveThread, this);
}

void RtpUncompressedDepayloader::Stop() {
  // Call the base class
  RtpDepayloader::Stop();
  rx_thread_running_ = false;

  if (rx_thread_.joinable()) {
    rx_thread_.join();
  }
}

bool RtpUncompressedDepayloader::WaitForFrame(uint8_t **cpu, int32_t timeout) {
  *cpu = GetBuffer().data();
  // Wait for completion
  if (timeout <= 0) {
    // Block till new frame
    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return true;
  } else {
    // Timeout
    auto to = std::chrono::milliseconds(timeout);
    auto start_time = std::chrono::high_resolution_clock::now();
    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      auto end_time = std::chrono::high_resolution_clock::now();

      if (auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
          duration >= to) {
        memset(GetBuffer().data(), 0, GetBuffer().size());
        return false;
      }
    }

    return true;
  }
}

bool RtpUncompressedDepayloader::Receive(::mediax::rtp::RtpFrameData *data, int32_t timeout) {
  if (GetStream().port_no == 0) {
    LOG(ERROR) << "Port number has not been set";
    return false;
  }

  if (GetStream().settings_valid == false) {
    LOG(ERROR) << "IP settings are invalid";
    return false;
  }

  // Check ports open
  if ((GetStream().socket_open == false) && (GetStream().settings_valid == true)) {
    Open();
  }

  data->resolution.height = GetHeight();
  data->resolution.width = GetWidth();
  data->encoding = GetColourSpace();

  // Check if we have a frame ready
  if (new_rx_frame_) {
    // Dont start a new thread if a frame is available just return it
    data->cpu_buffer = GetBuffer().data();
    new_rx_frame_ = false;
    return true;
  } else {
    bool ret = WaitForFrame(&data->cpu_buffer, timeout);
    return ret;
  }

  // should not ever get here
  LOG(ERROR) << "Something went wrong should not be here";
  return false;
}

void RtpUncompressedDepayloader::Callback(::mediax::rtp::RtpFrameData frame) const {
  if (GetState() == ::mediax::rtp::StreamState::kStarted) {
    GetCallback()(static_cast<const RtpDepayloader &>(*this), frame);
  }
}

}  // namespace mediax::rtp::uncompressed
