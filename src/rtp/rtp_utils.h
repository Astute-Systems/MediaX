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

#ifndef RTP_RTP_UTILS_H_
#define RTP_RTP_UTILS_H_

#ifdef _WIN32
#include <winsock2.h>
// Swap bytes in 16 bit value.
#define __bswap_constant_16(x) ((((x) >> 8) & 0xffu) | (((x)&0xffu) << 8))
// Swap bytes in 32 bit value.
#define __bswap_constant_32(x) \
  ((((x) >> 24) & 0xffu) | (((x)&0xff0000u) >> 8) | (((x)&0xff00u) << 8) | (((x)&0xffu) << 24))
#define __bswap_16(x) __bswap_constant_16(x)
#else
#include <byteswap.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
// #include <stdint.h>

namespace mediax {

///
/// \brief Initialize the RTP library, mainly needed for GStreamer support
///
/// \param argc
/// \param argv
///
void RtpInit(int argc, char *argv[]);

///
/// \brief Finalise the RTP library, mainly needed for GStreamer support
///
///
void RtpCleanup();

}  // namespace mediax

///
/// \brief Swap the endianness of a 32-bit integer
///
/// \param data A pointer to the data
/// \param length The length of the data
///
void EndianSwap32(uint32_t *data, unsigned int length);

///
/// \brief Swap the endianness of a 16-bit integer
///
/// \param data A pointer to the data
/// \param length The length of the data
///
void EndianSwap16(uint16_t *data, unsigned int length);

///
/// \brief Dump a hex representation of a buffer
///
/// \param data the buffer to write the test card to
/// \param size the size of the buffer
///
void DumpHex(const void *data, size_t size);

///
/// \brief Create a Colour Bar Test Card object
///
/// \param data the buffer to write the test card to
/// \param width the image width in pixels
/// \param height the image height in pixels
///
void CreateColourBarTestCard(uint8_t *data, uint32_t width, uint32_t height);

///
/// \brief Create a Grey Scale Bar Test Card object
///
/// \param data the buffer to write the test card to
/// \param width the image width in pixels
/// \param height the image height in pixels
///
void CreateGreyScaleBarTestCard(uint8_t *data, uint32_t width, uint32_t height);

///
/// \brief Create a Smtpe Test Card object
///
/// \param data the buffer to write the test card to
/// \param width the image width in pixels
/// \param height the image height in pixels
///
void CreateComplexTestCard(uint8_t *data, uint32_t width, uint32_t height);

///
/// \brief Create a Checkerd Test Card object
///
/// \param data the buffer to write the test card to
/// \param width the image width in pixels
/// \param height the image height in pixels
///
void CreateCheckeredTestCard(uint8_t *data, uint32_t width, uint32_t height);

///
/// \brief Create a Solid Test Card object
///
/// \param data the buffer to write the test card to
/// \param width the image width in pixels
/// \param height the image height in pixels
/// \param red RGB value
/// \param green RGB value
/// \param blue RGB value
///
void CreateSolidTestCard(uint8_t *data, uint32_t width, uint32_t height, uint8_t red, uint8_t green, uint8_t blue);

#endif  // RTP_RTP_UTILS_H_
