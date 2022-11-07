/* Example RTP packet from wireshark
      Real-Time Transport Protocol
      10.. .... = Version: RFC 1889 Version (2)
      ..0. .... = Padding: False
      ...0 .... = Extension: False
      .... 0000 = Contributing source identifiers count: 0
      0... .... = Marker: False
      Payload type: DynamicRTP-Type-96 (96)
      Sequence number: 34513
      Timestamp: 2999318601
      Synchronization Source identifier: 0xdccae7a8 (3704285096)
      Payload: 000003c000a08000019e00a2000029292929f06e29292929...
*/

/*

Gstreamer1.0 working example UYVY streaming
===========================================
gst-launch-1.0 videotestsrc num_buffers ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue !
rtpvrawpay ! udpsink host=127.0.0.1 port=5004

gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480,
payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false


Use his program to stream data to the udpsc example above on the tegra X1

*/

#ifndef __RTP_STREAM_H__
#define __RTP_STREAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if __MINGW64__ || __MINGW32__
#include <winsock2.h>
// Swap bytes in 16 bit value.
#define __bswap_constant_16(x) ((((x) >> 8) & 0xffu) | (((x)&0xffu) << 8))
// Swap bytes in 32 bit value.
#define __bswap_constant_32(x) \
  ((((x)&0xff000000u) >> 24) | (((x)&0x00ff0000u) >> 8) | (((x)&0x0000ff00u) << 8) | (((x)&0x000000ffu) << 24))
#define __bswap_32(x)                  \
  (__extension__({                     \
    register unsigned int __bsx = (x); \
    __bswap_constant_32(__bsx);        \
  }))
#define __bswap_16(x)               \
  (__extension__({                  \
    unsigned short int __bsx = (x); \
    __bswap_constant_16(__bsx);     \
  }))
#else
#include <byteswap.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include <limits.h>

#define GST_1_FIX 1
#define ENDIAN_SWAP __amd64__ || __x86_64__  // Perform endian swap, Intel defined by gcc
#define RTP_VERSION 0x2                      // RFC 1889 Version 2
#define RTP_PADDING 0x0
#define RTP_EXTENSION 0x0
#define RTP_MARKER 0x0
#define RTP_PAYLOAD_TYPE 0x60  // 96 Dynamic Type
#define RTP_SOURCE 0x12345678  // Sould be unique
#define RTP_FRAMERATE 25

#define Hz90 90000
#define NUM_LINES_PER_PACKET 10  // can have more that one line in a packet
#define MAX_BUFSIZE 1280 * 3     // allow for RGB data upto 1280 pixels wide
#define MAX_UDP_DATA 1500        // enough space for three lines of UDP data MTU size should be checked

// 12 byte RTP Raw video header
typedef struct __attribute__((__packed__)) {
  int32_t protocol : 32;
  int32_t timestamp : 32;
  int32_t source : 32;
} RtpHeader;

typedef struct __attribute__((__packed__)) {
  int16_t length : 16;
  int16_t line_number : 16;
  int16_t offset : 16;
} LineHeader;

typedef struct __attribute__((__packed__)) {
  int16_t extended_sequence_number : 16;
  LineHeader line[NUM_LINES_PER_PACKET];  // TODO (ross@rossnewman.com): This can be multiline.
} PayloadHeader;

typedef struct __attribute__((__packed__)) {
  RtpHeader rtp;
  PayloadHeader payload;
} Header;

typedef struct __attribute__((__packed__)) {
  Header head;
  char data[MAX_BUFSIZE];
} RtpPacket;

void yuvtorgb(int height, int width, char *yuv, char *rgba);
void rgbtoyuv(int height, int width, char *rgb, char *yuv);
void yuvtorgba(int height, int width, char *yuv, char *rgba);
void yuvtorgb(int height, int width, char *yuv, char *rgb);

//
// rtpstream RGB data
//
class RtpStream {
 public:
  RtpStream(int height, int width);
  ~RtpStream();
  static unsigned long sequence_number_;
  void RtpStreamOut(char *hostname, int port);
  void RtpStreamIn(char *hostname, int port);
  int Transmit(char *rgbframe);
  bool Open();
  void Close();
  bool Recieve(void **cpu, unsigned long timeout = ULONG_MAX);
  int sockfd_in_;
  int sockfd_out_;
  struct sockaddr_in server_addr_in_;
  struct sockaddr_in server_addr_out_;
  socklen_t server_len_in_;
  socklen_t server_len_out_;
  pthread_mutex_t mutex_;
  unsigned int frame_;
  char *gpuBuffer;
  char udpdata[MAX_UDP_DATA];
  char *buffer_in_;
  void UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source);

 private:
  struct hostent *server_in_;
  struct hostent *server_out_;
  int height_;
  int width_;
  // Ingress port
  char hostname_in_[100];
  int port_no_in_;
  // Egress port
  char hostname_out_[100];
  int port_no_out_;
};

//
// Transmit data structure
//
typedef struct {
  char *rgbframe;
  char *yuvframe;
  uint32_t width;
  uint32_t height;
  RtpStream *stream;
} TxData;

static TxData arg_rx;

#endif
