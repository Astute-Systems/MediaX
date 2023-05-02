#include "rtp_utils.h"

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
