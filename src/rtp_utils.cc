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
/// \brief RTP utility functions
///
/// \file rtp_utils.cc

#include "rtp_utils.h"

///
/// \brief Swap the endianness of a 32-bit integer
///
/// \param data A pointer to the data
/// \param length The length of the data
///
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

///
/// \brief Swap the endianness of a 16-bit integer
///
/// \param data A pointer to the data
/// \param length The length of the data
///
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
