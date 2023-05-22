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
/// \file rtp_utils.h

#ifndef RTP_UTILS_H
#define RTP_UTILS_H

#ifdef _WIN32
#include <winsock2.h>
// Swap bytes in 16 bit value.
#define __bswap_constant_16(x) ((((x) >> 8) & 0xffu) | (((x)&0xffu) << 8))
// Swap bytes in 32 bit value.
#define __bswap_constant_32(x) ((((x) >> 24) & 0xffu) | (((x)&0xff0000u) >> 8) | (((x)&0xff00u) << 8) | (((x)&0xffu) << 24))
#define __bswap_16(x) __bswap_constant_16(x)
#else
#include <byteswap.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include <stdint.h>

void EndianSwap32(uint32_t *data, unsigned int length);
void EndianSwap16(uint16_t *data, unsigned int length);

#endif  // RTP_UTILS_H
