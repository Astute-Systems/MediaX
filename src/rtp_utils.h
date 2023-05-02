
#ifndef RTP_UTILS_H
#define RTP_UTILS_H

#if _WIN32
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

void EndianSwap32(uint32_t *data, unsigned int length);
void EndianSwap16(uint16_t *data, unsigned int length);

#endif  // RTP_UTILS_H
