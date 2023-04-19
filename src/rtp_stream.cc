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
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
using namespace std;

uint32_t RtpStream::sequence_number_ = 0;
uint16_t RtpStream::extended_sequence_number_ = 0;

// static TxData arg_rx;
// error - wrapper for perror
void error(const std::string &msg) {
  perror(msg.c_str());
  exit(0);
}

RtpStream::RtpStream(uint32_t height, uint32_t width) : height_(height), width_(width) {
  pthread_mutex_init(&mutex_, NULL);
  buffer_in_ = (uint8_t *)malloc(height * width * 2);  // Holds YUV data
}

RtpStream::~RtpStream(void) { free(buffer_in_); }

/* Broadcast the stream to port i.e. 5004 */
void RtpStream::RtpStreamIn(std::string_view hostname, const int portno) {
  port_no_in_ = portno;
  hostname_in_ = hostname;
}

void RtpStream::RtpStreamOut(std::string_view hostname, const int portno) {
  port_no_out_ = portno;
  hostname_out_ = hostname;
}

bool RtpStream::Open() {
  if (port_no_in_) {
    struct sockaddr_in si_me;

    // create a UDP socket
    if ((sockfd_in_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      cout << "ERROR opening socket\n";
      return false;
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port_no_in_);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port
    if (bind(sockfd_in_, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
      cout << "ERROR binding socket\n";
      return false;
    }
  }

  if (port_no_out_) {
    // socket: create the outbound socket
    sockfd_out_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd_out_ < 0) {
      cout << "ERROR opening socket\n";
      return false;
    }

    // gethostbyname: get the server's DNS entry
    getaddrinfo(hostname_out_.c_str(), nullptr, nullptr, &server_out_);
    if (server_out_ == nullptr) {
      fprintf(stderr, "ERROR, no such host as %s\n", hostname_out_.c_str());
      exit(0);
    }

    // build the server's Internet address
    server_addr_out_.sin_family = AF_INET;
    server_addr_out_.sin_port = htons(port_no_out_);

    // send the message to the server
    server_len_out_ = sizeof(server_addr_out_);
#if 0
    int n = sendto(sockfd_out_, (char *) "hello", 5, 0,
                   (const sockaddr *) &server_addr_out_, server_len_out_);

    printf("n=%d\n", n);
    if (n < 0) {
      printf("[RTP] Transmit socket failure fd=%d\n", sockfd_out_);
      return n;
    }
#endif
  }
  return true;
}

void RtpStream::Close() const {
  if (port_no_in_) {
    close(sockfd_in_);
  }

  if (port_no_out_) {
    close(sockfd_out_);
  }
}

#if ENDIAN_SWAP
void EndianSwap32(uint32_t *data, unsigned int length) {
  for (uint32_t c = 0; c < length; c++) data[c] = __bswap_32(data[c]);
}

void EndianSwap16(uint16_t *data, unsigned int length) {
  for (uint16_t c = 0; c < length; c++) data[c] = __bswap_16(data[c]);
}
#endif

void RtpStream::UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source) const {
  memset((char *)packet, 0, sizeof(Header));
  packet->rtp.protocol = kRtpVersion << 30;
  packet->rtp.protocol = (kRtpExtension << 28) | packet->rtp.protocol;
  packet->rtp.protocol = packet->rtp.protocol | kRtpPayloadType << 16;
  packet->rtp.protocol = packet->rtp.protocol | sequence_number_++;
  timestamp += (Hz90 / kRtpFramerate);
  packet->rtp.timestamp = timestamp;
  packet->rtp.source = source;
  packet->payload.extended_sequence_number = extended_sequence_number_++;
  packet->payload.line[0].length = (int16_t)width_ * 2;
  packet->payload.line[0].line_number = (int16_t)line;
  packet->payload.line[0].offset = 0;
  if (last == 1) {
    packet->rtp.protocol = packet->rtp.protocol | 1 << 23;
  }
}

void RtpStream::ReceiveThread(RtpStream *stream) {
  RtpPacket *packet;
  bool receiving = true;
  int scan_count = 0;
  int last_packet;

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
      recvfrom(stream->sockfd_in_, stream->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr);

      packet = (RtpPacket *)(stream->udpdata.data());
#if ENDIAN_SWAP
      EndianSwap32((uint32_t *)(packet), sizeof(RtpHeader) / 4);
#endif
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

#if ENDIAN_SWAP
        EndianSwap16((uint16_t *)(&packet->head.payload.line[scan_count]), sizeof(LineHeader) / 2);
#endif
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
                ((packet->head.payload.line[c].line_number & 0x7FFF) * (stream->width_ * 2));
        length = packet->head.payload.line[c].length & 0xFFFF;

        memcpy(&stream->buffer_in_[pixel], &stream->udpdata[os], length);

        last_packet += length;
        payload += length;
      }

      if (marker) receiving = false;

      scan_count = 0;
    }
  }

  stream->arg_tx.yuvframe = stream->buffer_in_;

  return;
}

bool RtpStream::Receive(uint8_t **cpu, uint32_t timeout [[maybe_unused]]) {
  if (kRtpThreaded) {
    // Elevate priority to get the RTP packets in quickly

    // Start a thread so we can start capturing the next frame while transmitting the data
    rx_thread_ = std::thread(&RtpStream::ReceiveThread, this);

    // Wait for completion
    rx_thread_.join();

  } else {
    ReceiveThread(this);
  }
  *cpu = buffer_in_;
  return true;
}

void RtpStream::TransmitThread(RtpStream *stream) {
  RtpPacket packet;

  ssize_t n = 0;

  int32_t stride = stream->width_ * 2;

  RtpStream::sequence_number_ = 0;

  /* send a frame */
  pthread_mutex_lock(&stream->mutex_);
  {
    uint32_t time = 10000;

    for (uint32_t c = 0; c < (stream->height_); c++) {
      uint32_t last = 0;

      if (c == stream->height_ - 1) last = 1;
      stream->UpdateHeader((Header *)&packet, c, last, time, kRtpSource);

#if ENDIAN_SWAP
      EndianSwap32((uint32_t *)(&packet), sizeof(RtpHeader) / 4);
      EndianSwap16((uint16_t *)(&packet.head.payload), sizeof(PayloadHeader) / 2);
#endif

      memcpy((void *)&packet.head.payload.line[1], (void *)&stream->arg_tx.rgbframe[(c * stride) + 1], stride);
      n = sendto(stream->sockfd_out_, (uint8_t *)&packet, stride + 20, 0, (const sockaddr *)&stream->server_addr_out_,
                 stream->server_len_out_);

      if (n == 0) {
        cerr << "[RTP] Transmit socket failure fd=" << stream->sockfd_out_ << "\n";
        return;
      }
    }
  }
  pthread_mutex_unlock(&stream->mutex_);
  return;
}

int RtpStream::Transmit(uint8_t *rgbframe) {
  arg_tx.rgbframe = rgbframe;

  if (kRtpThreaded) {
    // Start a thread so we can start capturing the next frame while transmitting the data
    tx_thread_ = std::thread(TransmitThread, this);
    tx_thread_.detach();
  } else {
    TransmitThread(this);
  }

  return 0;
}
