//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP utility functions
///
/// \file rtp_utils.cc

#include "rtp_utils.h"

#include <iomanip>
#include <iostream>

void EndianSwap32(uint32_t *data, unsigned int length) {
  // Check for Intel architecture
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#ifdef _WIN32
  for (uint32_t c = 0; c < length; c++) data[c] = _byteswap_ulong(data[c]);
#else
  for (uint32_t c = 0; c < length; c++) data[c] = __bswap_32(data[c]);
#endif
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

void DumpHex(const void *data, size_t size) {
  const unsigned char *p = static_cast<const unsigned char *>(data);
  std::cout << std::hex << std::setfill('0');
  for (size_t i = 0; i < size; ++i) {
    std::cout << std::setw(2) << static_cast<unsigned int>(p[i]) << ' ';
    if ((i + 1) % 16 == 0) {
      std::cout << "  ";
      for (size_t j = i - 15; j <= i; ++j) {
        if (std::isprint(p[j])) {
          std::cout << static_cast<char>(p[j]);
        } else {
          std::cout << '.';
        }
      }
      std::cout << std::endl;
    }
  }
  if (size % 16 != 0) {
    size_t remaining = size % 16;
    for (size_t i = 0; i < 16 - remaining; ++i) {
      std::cout << "   ";
    }
    std::cout << "  ";
    for (size_t i = size - remaining; i < size; ++i) {
      if (std::isprint(p[i])) {
        std::cout << static_cast<char>(p[i]);
      } else {
        std::cout << '.';
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::dec << std::endl;
}
