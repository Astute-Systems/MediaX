//
// MIT License
//
// Copyright (c) 2022 Ross Newman (ross.newman@defencex.com.au)
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
#if __MINGW64__ || __MINGW32__
#include <WS2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#endif
extern "C" {
#include "libswscale/swscale.h"
}
#include "rtp_stream.h"
using namespace std;

#define RTP_CHECK 0     // 0 to disable RTP header checking
#define RTP_THREADED 1  // transmit and recieve in a thread. RX thread blocks TX does not
#define PITCH 4         // RGBX processing pitch

#if ENDIAN_SWAP
void EndianSwap32(uint32_t *data, unsigned int length);
void EndianSwap16(uint16_t *data, unsigned int length);
#endif

typedef struct float4 {
  float x;
  float y;
  float z;
  float w;
} float4;

// error - wrapper for perror
void error(char *msg) {
  perror(msg);
  exit(0);
}

void YuvToRgb(int height, int width, char *yuv, char *rgba) {
  SwsContext *ctx =
      sws_getContext(width, height, AV_PIX_FMT_YUYV422, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)yuv};    // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)rgba};  // YUYV have one plane
  int inLinesize[1] = {width * 2};          // YUYV stride
  int outLinesize[1] = {width * 3};         // RGB srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void YuvToRgba(int height, int width, char *yuv, char *rgb) {
  SwsContext *ctx =
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)yuv};   // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)rgb};  // YUYV have one plane
  int inLinesize[1] = {width * 2};         // YUYV stride
  int outLinesize[1] = {width * 4};        // RGB srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void RgbaToYuv(int height, int width, char *rgba, char *yuv) {
  SwsContext *ctx = sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_YUYV422, 0, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)rgba};  // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)yuv};  // YUYV have one plane
  int inLinesize[1] = {width * 4};         // RGB stride
  int outLinesize[1] = {width * 2};        // YUYV srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void RgbToYuv(int height, int width, char *rgb, char *yuv) {
  SwsContext *ctx = sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_YUYV422, 0, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)rgb};   // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)yuv};  // YUYV have one plane
  int inLinesize[1] = {width * 3};         // RGB stride
  int outLinesize[1] = {width * 2};        // YUYV srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

unsigned long RtpStream::sequence_number_;

RtpStream::RtpStream(int height, int width) {
  height_ = height;
  width_ = width;
  frame_ = 0;
  port_no_in_ = 0;
  port_no_out_ = 0;
  sequence_number_ = 0;
  pthread_mutex_init(&mutex_, NULL);
  buffer_in_ = (char *)malloc(height * width * 2);  // Holds YUV data
  cout << "[RTP] RtpStream created << " << width_ << "x" << height_ << "\n";
}

RtpStream::~RtpStream(void) { free(buffer_in_); }

/* Broadcast the stream to port i.e. 5004 */
void RtpStream::RtpStreamIn(const char *hostname, const int portno) {
  cout << "[RTP] RtpStreamIn " << hostname << ":" << portno << "\n";
  port_no_in_ = portno;
  strncpy(hostname_in_, hostname, sizeof(hostname_in_));
}

void RtpStream::RtpStreamOut(const char *hostname, const int portno) {
  cout << "[RTP] RtpStreamOut " << hostname << ":" << portno << "\n";
  port_no_out_ = portno;
  strncpy(hostname_out_, hostname, sizeof(hostname_in_));
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
#if RTP_MULTICAST
    {
      struct ip_mreq multi;

      // Multicast
      multi.imr_multiaddr.s_addr = inet_addr(IP_MULTICAST_IN);
      multi.imr_interface.s_addr = htonl(INADDR_ANY);
      if (setsockopt(sockfd_in_, IPPROTO_UDP, IP_ADD_MEMBERSHIP, &multi, sizeof(multi)) < 0) {
        cout << "ERROR failed to join multicast group " << IP_MULTICAST_IN << "\n";
      }
    }
#endif
  }

  if (port_no_out_) {
    /* socket: create the outbound socket */
    sockfd_out_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd_out_ < 0) {
      cout << "ERROR opening socket\n";
      return false;
    }

    /* gethostbyname: get the server's DNS entry */
    server_out_ = gethostbyname(hostname_out_);
    if (server_out_ == NULL) {
      fprintf(stderr, "ERROR, no such host as %s\n", hostname_out_);
      exit(0);
    }

    /* build the server's Internet address */
    bzero((char *)&server_addr_out_, sizeof(server_addr_out_));
    server_addr_out_.sin_family = AF_INET;
    bcopy((char *)server_out_->h_addr, (char *)&server_addr_out_.sin_addr.s_addr, server_out_->h_length);
    server_addr_out_.sin_port = htons(port_no_out_);

    /* send the message to the server */
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

void RtpStream::Close() {
  if (port_no_in_) {
    close(sockfd_in_);
  }

  if (port_no_out_) {
    close(sockfd_out_);
  }
}

#if ENDIAN_SWAP
void EndianSwap32(uint32_t *data, unsigned int length) {
  uint32_t c = 0;

  for (c = 0; c < length; c++) data[c] = __bswap_32(data[c]);
}

void EndianSwap16(uint16_t *data, unsigned int length) {
  uint16_t c = 0;

  for (c = 0; c < length; c++) data[c] = __bswap_16(data[c]);
}
#endif
void RtpStream::UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source) {
  bzero((char *)packet, sizeof(Header));
  packet->rtp.protocol = RTP_VERSION << 30;
  packet->rtp.protocol = packet->rtp.protocol | RTP_PAYLOAD_TYPE << 16;
  packet->rtp.protocol = packet->rtp.protocol | sequence_number_++;
  /* leaving other fields as zero TODO Fix */
  packet->rtp.timestamp = timestamp += (Hz90 / RTP_FRAMERATE);
  packet->rtp.source = source;
  packet->payload.extended_sequence_number = 0; /* TODO : Fix extended seq numbers */
  packet->payload.line[0].length = width_ * 2;
  packet->payload.line[0].line_number = line;
  packet->payload.line[0].offset = 0;
  if (last == 1) {
    packet->rtp.protocol = packet->rtp.protocol | 1 << 23;
  }
}

void *ReceiveThread(void *data) {
  TxData *arg;
  RtpPacket *packet;
  bool receiving = true;
  int scancount = 0;
  int lastpacket;

  arg = (TxData *)data;

  while (receiving) {
    int marker;

#if RTP_CHECK
    int version;
    int payloadType;
    int seqNo, last;
#endif
    bool valid = false;

    //
    // Read data until we get the next RTP header
    //
    while (!valid) {
      //
      // Read in the RTP data
      //
      recvfrom(arg->stream->sockfd_in_, arg->stream->udpdata, MAX_UDP_DATA, 0, NULL, NULL);

      packet = reinterpret_cast<RtpPacket *>(arg->stream->udpdata);
#if ENDIAN_SWAP
      EndianSwap32(reinterpret_cast<uint32_t *>(packet), sizeof(RtpHeader) / 4);
#endif
      //
      // Decode Header bits and confirm RTP packet
      //
#if RTP_CHECK
      payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
      version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
      seqNo = (packet->head.rtp.protocol & 0x0000FFFF);
      if ((payloadType == 96) && (version == 2))
#endif
      {
#if 0
        if (seqNo != last + 1) {

          printf("Dropped %d packets (%d to %d)\n", seqNo - last, last, seqNo);
          last = seqNo;
        }
#else
        valid = true;
#endif
      }
    }
    if (valid) {  // Start to decode packet
      bool scanline = true;

      // Decode Header bits
      marker = (packet->head.rtp.protocol & 0x00800000) >> 23;
#if RTP_CHECK
      printf("[RTP] seqNo %d, Packet %d, marker %d, Rx length %d, timestamp 0x%08x\n", seqNo, payloadType, marker, len,
             packet->head.rtp.timestamp);
#endif

      //
      // Count the number of scanlines in the packet
      //
      while (scanline) {
        int more;

#if ENDIAN_SWAP
        EndianSwap16(reinterpret_cast<uint16_t *>(&packet->head.payload.line[scancount]), sizeof(LineHeader) / 2);
#endif
        more = (packet->head.payload.line[scancount].offset & 0x8000) >> 15;
        if (!more) scanline = false;  // The last scanline
        scancount++;
      }

      //
      // Now we know the number of scanlines we can copy the data
      //
      int payloadoffset = sizeof(RtpHeader) + 2 + (scancount * sizeof(LineHeader));
      int payload = 0;

      lastpacket = payloadoffset;
      for (int c = 0; c < scancount; c++) {
        uint32_t os;
        uint32_t pixel;
        uint32_t length;

        os = payloadoffset + payload;
        pixel = ((packet->head.payload.line[c].offset & 0x7FFF) * 2) +
                ((packet->head.payload.line[c].line_number & 0x7FFF) * (arg->width * 2));
        length = packet->head.payload.line[c].length & 0xFFFF;
        if (pixel < 3000)
          printf("po=%d, p=%d, os=%d, Sc = %d, pixel=%d, length=%d\n", payloadoffset, payload, os, c, pixel, length);

#ifdef GST_1_FIX  // UYVY is VYUY (GStreamer bug?) need to swop.
        for (uint32_t fix = 0; fix < length; fix += 4) {
          unsigned char tmp = arg->stream->udpdata[os + fix + 2];
          arg->stream->udpdata[os + fix + 2] = arg->stream->udpdata[os + fix];
          arg->stream->udpdata[os + fix] = tmp;
        }
#endif
        memcpy(&arg->stream->buffer_in_[pixel], &arg->stream->udpdata[os], length);

        lastpacket += length;
        payload += length;
      }

      if (marker) receiving = false;

      scanline = true;
      scancount = 0;
    }
  }

  arg->yuvframe = arg->stream->buffer_in_;

  return 0;
}

bool RtpStream::Receive(void **cpu, unsigned long timeout) {
  sched_param param;
  pthread_attr_t tattr;
  pthread_t rx;
  TxData arg_rx;

  arg_rx.rgbframe = 0;
  arg_rx.width = width_;
  arg_rx.height = height_;
  arg_rx.stream = this;

#if RTP_THREADED
  // Elevate priority to get the RTP packets in quickly
  pthread_attr_init(&tattr);
  pthread_attr_getschedparam(&tattr, &param);
  param.sched_priority = 99;
  pthread_attr_setschedparam(&tattr, &param);

  // Start a thread so we can start capturing the next frame while transmitting the data
  pthread_create(&rx, &tattr, ReceiveThread, &arg_rx);

  // Wait for completion
  pthread_join(rx, 0);
#else
  ReceiveThread(&arg_rx);
#endif
  *cpu = (void *)buffer_in_;
  return true;
}

void *TransmitThread(void *data) {
  RtpPacket packet;
  TxData *arg;
  uint32_t c = 0;
  uint32_t n = 0;

  arg = (TxData *)data;

  RtpStream::sequence_number_ = 0;

  /* send a frame */
  pthread_mutex_lock(&arg->stream->mutex_);
  {
    uint32_t time = 10000;

    for (c = 0; c < (arg->height); c++) {
      uint32_t last = 0;

      if (c == arg->height - 1) last = 1;
      arg->stream->UpdateHeader((Header *)&packet, c, last, time, RTP_SOURCE);

#if ENDIAN_SWAP
      EndianSwap32(reinterpret_cast<uint32_t *>(&packet), sizeof(RtpHeader) / 4);
      EndianSwap16(reinterpret_cast<uint16_t *>(&packet.head.payload), sizeof(PayloadHeader) / 2);
#endif
      n = sendto(arg->stream->sockfd_out_, (char *)&packet, 24 + (arg->width * 2), 0,
                 (const sockaddr *)&arg->stream->server_addr_out_, arg->stream->server_len_out_);

      if (n < 0) {
        cout << "[RTP] Transmit socket failure fd=" << arg->stream->sockfd_out_ << "\n";
        return 0;
      }
    }
  }
  pthread_mutex_unlock(&arg->stream->mutex_);
  return 0;
}

// Arguments sent to thread
sched_param param;
pthread_t tx;
static TxData arg_tx;

int RtpStream::Transmit(const char *rgbframe) {
  arg_tx.rgbframe = const_cast<char *>(rgbframe);
  arg_tx.width = width_;
  arg_tx.height = height_;
  arg_tx.stream = this;

#if RTP_THREADED
  // Start a thread so we can start capturing the next frame while transmitting the data
  pthread_join(tx, 0);
  pthread_create(&tx, NULL, TransmitThread, &arg_tx);
  return 0;  // Cant know the if the transmit was successfull if done in a thread
#else
  return TransmitThread(&arg_tx);
#endif
}
