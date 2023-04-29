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
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtp_stream.cc
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

#include "rtp_stream.h"
#include "rtp_types.h"
#include "sap_announcer.h"
using namespace std;

uint32_t RtpStream::sequence_number_ = 0;

PortType RtpStream::ingress_;
std::vector<uint8_t> RtpStream::buffer_in_;

// static TxData arg_rx;
// error - wrapper for perror
void error(const std::string &msg) {
  perror(msg.c_str());
  exit(0);
}

RtpStream::RtpStream() { pthread_mutex_init(&mutex_, nullptr); }

RtpStream::~RtpStream(void) = default;

// Broadcast the stream to port i.e. 5004
void RtpStream::RtpStreamIn(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                            std::string_view hostname, const uint32_t portno) {
  ingress_.encoding = encoding;
  ingress_.height = height;
  ingress_.width = width;
  ingress_.name = name;
  ingress_.hostname = hostname;
  ingress_.port_no = portno;
  ingress_.settings_valid = true;
  buffer_in_.resize(height * width * 2);
}

void RtpStream::RtpStreamOut(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
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

void RtpStream::SapCallback(sap::SDPMessage &sdp) {
  RtpStreamIn(sdp.session_name, ColourspaceType::kColourspaceYuv, sdp.height, sdp.width, sdp.ip_address, sdp.port);
}

void RtpStream::RtpStreamIn(std::string_view name) const {
  sap::SAPListener &sap = sap::SAPListener::GetInstance();
  sap.RegisterSapListener(name, SapCallback);
  sap::SAPListener::GetInstance().Start();
}

bool RtpStream::Open() {
  if (!ingress_.port_no && !egress_.port_no) {
    std::cerr << "No ports set, nothing to open";
    exit(-1);
  }
  if (ingress_.port_no) {
    struct sockaddr_in si_me;

    // create a UDP socket
    if ((ingress_.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      std::cerr << "ERROR opening socket\n";
      exit(-1);
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(ingress_.port_no);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if (bind(ingress_.sockfd, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
      std::cerr << "ERROR binding socket\n";
      exit(-1);
    }
    ingress_.socket_open = true;
  }

  if (egress_.port_no) {
    // socket: create the outbound socket
    egress_.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (egress_.sockfd < 0) {
      cout << "ERROR opening socket\n";
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
    server_addr_out_.sin_port = htons(egress_.port_no);

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

void RtpStream::Close() {
  sap::SAPListener::GetInstance().Stop();

  if (ingress_.port_no) {
    close(ingress_.sockfd);
    ingress_.sockfd = 0;
    ingress_.socket_open = false;
  }

  if (egress_.port_no) {
    close(egress_.sockfd);
    egress_.sockfd = 0;
    egress_.socket_open = false;
  }
}

void EndianSwap32(uint32_t *data, unsigned int length) {
  // Check for Intel architecture
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  for (uint32_t c = 0; c < length; c++) data[c] = __bswap_32(data[c]);
  return;
// Check for ARM architecture
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return;
#else
#error "Unknown byte order"
#endif
}

void EndianSwap16(uint16_t *data, unsigned int length) {
  // Check for Intel architecture
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  for (uint16_t c = 0; c < length; c++) data[c] = __bswap_16(data[c]);
  return;
// Check for ARM architecture
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return;
#else
#error "Unknown byte order"
#endif
}

void RtpStream::UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source) const {
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

bool RtpStream::new_rx_frame_ = false;
bool RtpStream::rx_thread_running_ = true;
void RtpStream::ReceiveThread(RtpStream *stream) {
  RtpPacket *packet;
  bool receiving = true;
  int scan_count = 0;
  int last_packet;

  while (rx_thread_running_) {
    while (receiving) {
      int marker;

      int version;
      int payloadType;
      bool valid = false;

      //
      // Read data until we get the next RTP header
      //
      while (!valid) {
        //
        // Read in the RTP data
        //
        if (ssize_t bytes =
                recvfrom(RtpStream::ingress_.sockfd, stream->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr);
            bytes <= 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          continue;
        }
        packet = (RtpPacket *)(stream->udpdata.data());
        EndianSwap32((uint32_t *)(packet), sizeof(RtpHeader) / 4);

        //
        // Decode Header bits and confirm RTP packet
        //
        payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
        version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
        if ((payloadType == 96) && (version == 2)) {
          valid = true;
        }
      }
      if (valid) {  // Start to decode packet
        bool scan_line = true;

        // Decode Header bits
        marker = (packet->head.rtp.protocol & 0x00800000) >> 23;

        //
        // Count the number of scan_lines in the packet
        //
        while (scan_line) {
          int more;
          EndianSwap16((uint16_t *)(&packet->head.payload.line[scan_count]), sizeof(LineHeader) / 2);
          more = (packet->head.payload.line[scan_count].offset & 0x8000) >> 15;
          !more ? scan_line = false : scan_line = true;  // The last scan_line
          scan_count++;
        }

        //
        // Now we know the number of scan_lines we can copy the data
        //
        int payload_offset = sizeof(RtpHeader) + 2 + (scan_count * sizeof(LineHeader));
        int payload = 0;

        last_packet = payload_offset;
        for (int c = 0; c < scan_count; c++) {
          uint32_t os;
          uint32_t pixel;
          uint32_t length;

          os = payload_offset + payload;
          pixel = ((packet->head.payload.line[c].offset & 0x7FFF) * 2) +
                  ((packet->head.payload.line[c].line_number & 0x7FFF) * (stream->ingress_.width * 2));
          length = packet->head.payload.line[c].length & 0xFFFF;

          memcpy(&stream->buffer_in_[pixel], &stream->udpdata[os], length);

          last_packet += length;
          payload += length;
        }

        marker ? receiving = false : receiving = true;

        scan_count = 0;
      }
    }

    stream->arg_tx.yuvframe = stream->buffer_in_.data();
    new_rx_frame_ = true;
    receiving = true;
  }  // Recieve loop
  return;
}

void RtpStream::Start() {
  rx_thread_running_ = true;
  rx_thread_ = std::thread(&RtpStream::ReceiveThread, this);
}

void RtpStream::Stop() {
  if (rx_thread_.joinable()) {
    rx_thread_running_ = false;
    rx_thread_.join();
  }
}

bool RtpStream::WaitForFrame(uint8_t **cpu, int32_t timeout) {
  // Wait for completion
  if (timeout < 0) {
    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  } else {
    auto to = std::chrono::milliseconds(timeout);
    auto start_time = std::chrono::high_resolution_clock::now();

    while (!new_rx_frame_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      auto end_time = std::chrono::high_resolution_clock::now();
      if (auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
          duration >= to) {
        // Leave the thread to recieve the rest of the frame
        return false;
      }
    }
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  }
}

bool RtpStream::Receive(uint8_t **cpu, int32_t timeout [[maybe_unused]]) {
  if (ingress_.port_no == 0) return false;
  if (ingress_.settings_valid == false) return false;

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
  return false;
}

void RtpStream::TransmitThread(RtpStream *stream) {
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

int RtpStream::Transmit(uint8_t *rgbframe, bool blocking) {
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

int32_t RtpStream::GenerateTimestamp90kHz() {
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