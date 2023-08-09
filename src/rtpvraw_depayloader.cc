//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtpvraw_depayloader.cc
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

#include <va/va.h>
#include <va/va_drm.h>
#include <va/va_x11.h>

#include <iostream>
#include <stdexcept>
#include <vector>

#include "rtp_types.h"
#include "rtp_utils.h"
#include "rtpvraw_depayloader.h"
#include "sap_announcer.h"

#define VA_MAX_PLANES 3

void DecodeFrame(VADisplay va_display, VAContextID va_context, VASurfaceID va_surface, const uint8_t *frame_data,
                 int frame_size) {
  VAConfigID va_config;
  VAStatus va_status;
  VABufferID va_buffer;
  VASurfaceStatus va_surface_status;
  VAImage va_image;
  void *va_image_data;
  uint32_t va_image_data_size;
  uint32_t va_image_pitch;
  uint32_t va_image_height;

  va_status = vaCreateConfig(va_display, VAProfileH264High, VAEntrypointVLD, nullptr, 0, &va_config);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create VA config");
  }

  auto type = (VABufferType)0;
  va_status = vaCreateBuffer(va_display, va_context, type, 0, 0, nullptr, &va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create VA buffer");
  }

  va_status = vaCreateSurfaces(va_display, VA_RT_FORMAT_YUV420, 640, 480, &va_surface, 1, nullptr, 0);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create VA surface");
  }

  va_status = vaCreateContext(va_display, va_config, 0, 0, 0, &va_surface, 1, &va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create VA context");
  }

  va_status = vaBeginPicture(va_display, va_context, va_surface);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to begin VA picture");
  }

  va_status = vaCreateBuffer(va_display, va_context, VASliceDataBufferType, frame_size, 1,
                             const_cast<uint8_t *>(frame_data), &va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create VA buffer");
  }

  va_status = vaRenderPicture(va_display, va_context, &va_buffer, 1);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to render VA picture");
  }

  va_status = vaEndPicture(va_display, va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to end VA picture");
  }

  va_status = vaSyncSurface(va_display, va_surface);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to sync VA surface");
  }

  va_status = vaDeriveImage(va_display, va_surface, &va_image);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to derive VA image");
  }

  va_status = vaMapBuffer(va_display, va_image.buf, &va_image_data);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to map VA buffer");
  }

  // va_status = vaQueryImage(va_display, va_image.image_id, &va_image_fourcc, &va_image_width, &va_image_height);
  // if (va_status != VA_STATUS_SUCCESS) {
  //   throw std::runtime_error("Failed to query VA image");
  // }

  va_image_pitch = va_image.pitches[0];

  // Do something with the decoded image data

  va_status = vaUnmapBuffer(va_display, va_image.buf);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to unmap VA buffer");
  }

  va_status = vaDestroyImage(va_display, va_image.image_id);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to destroy VA image");
  }

  va_status = vaDestroyBuffer(va_display, va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to destroy VA buffer");
  }

  va_status = vaDestroyContext(va_display, va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to destroy VA context");
  }

  va_status = vaDestroySurfaces(va_display, &va_surface, 1);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to destroy VA surface");
  }

  va_status = vaDestroyConfig(va_display, va_config);
  if (va_status != VA_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to destroy VA config");
  }
}

std::vector<uint8_t> RtpvrawDepayloader::buffer_in_;

RtpvrawDepayloader::RtpvrawDepayloader() { pthread_mutex_init(&mutex_, nullptr); }

RtpvrawDepayloader::~RtpvrawDepayloader(void) = default;

// Broadcast the stream to port i.e. 5004
void RtpvrawDepayloader::SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                                       std::string_view hostname, const uint32_t portno) {
  ingress_.encoding = encoding;
  ingress_.height = height;
  ingress_.width = width;
  ingress_.name = name;
  ingress_.hostname = hostname;
  ingress_.port_no = portno;
  ingress_.settings_valid = true;
  std::cout << "RtpvrawDepayloader::SetStreamInfo() " << ingress_.name << " " << ingress_.hostname << " "
            << ingress_.port_no << " " << ingress_.height << " " << ingress_.width << " "
            << std::to_string((int)ingress_.encoding) << "\n";
  buffer_in_.resize((height * width) * kColourspaceBytes.at(ingress_.encoding));
}

void RtpvrawDepayloader::SapCallback(const sap::SDPMessage &sdp) {
  SetStreamInfo(sdp.session_name, ColourspaceType::kColourspaceYuv, sdp.height, sdp.width, sdp.ip_address, sdp.port);
}

void RtpvrawDepayloader::SetStreamInfo(std::string_view name) const {
  sap::SAPListener &sap = sap::SAPListener::GetInstance();
  sap.RegisterSapListener(name, SapCallback);
  sap::SAPListener::GetInstance().Start();
}

bool RtpvrawDepayloader::Open() const {
  if (!ingress_.port_no) {
    std::cerr << "RtpvrawDepayloader::Open() No ports set, nothing to open";
    exit(-1);
  }
  if (ingress_.port_no) {
    struct sockaddr_in si_me;

    // create a UDP socket
    if ((ingress_.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      std::cerr << "RtpvrawDepayloader::Open() ERROR opening socket\n";
      exit(-1);
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons((uint16_t)ingress_.port_no);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if (bind(ingress_.sockfd, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
      std::cerr << "RtpvrawDepayloader::Open() ERROR binding socket\n";
      exit(-1);
    }
    ingress_.socket_open = true;
  }

  return true;
}

void RtpvrawDepayloader::Close() const {
  sap::SAPListener::GetInstance().Stop();

  if (ingress_.port_no) {
    close(ingress_.sockfd);
    ingress_.sockfd = 0;
    ingress_.socket_open = false;
  }
}

bool RtpvrawDepayloader::ReadRtpHeader(RtpvrawDepayloader *stream, RtpPacket *packet) const {
  int version;
  int payloadType;

  //
  // Read in the RTP data
  //
  if (ssize_t bytes =
          recvfrom(RtpDepayloader::ingress_.sockfd, stream->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr);
      bytes <= 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return false;
  }
  packet = (RtpPacket *)(stream->udpdata.data());
  EndianSwap32((uint32_t *)(packet), sizeof(RtpHeader) / 4);

  //
  // Decode Header bits and confirm RTP packet
  //
  payloadType = (packet->head.rtp.protocol & 0x007F0000) >> 16;
  version = (packet->head.rtp.protocol & 0xC0000000) >> 30;
  if ((payloadType == 96) && (version == 2)) {
    return true;
  }
  return false;
}

bool RtpvrawDepayloader::new_rx_frame_ = false;
bool RtpvrawDepayloader::rx_thread_running_ = true;
void RtpvrawDepayloader::ReceiveThread(RtpvrawDepayloader *stream) {
  RtpPacket *packet{};
  bool receiving = true;
  int scan_count = 0;
  int last_packet;

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(RtpDepayloader::ingress_.sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  while (rx_thread_running_) {
    while (receiving && rx_thread_running_) {
      int marker;

      int version;
      int payloadType;
      bool valid = false;

      //
      // Read data until we get the next RTP header
      //
      while (!valid && rx_thread_running_) {
        //
        // Read in the RTP data
        //

        if (ssize_t bytes =
                recvfrom(RtpDepayloader::ingress_.sockfd, stream->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr);
            bytes <= 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(2));
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
      if (valid) {
        // Valid packet so start to decode packet
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
          if (packet->head.payload.line[c].line_number == 0) {
            // Line numbers start at 1 in DEF-STAN 00-82, gstreamer starts at zero so drop those lines
            break;
          }
          pixel = ((packet->head.payload.line[c].offset & 0x7FFF) * kColourspaceBytes.at(ingress_.encoding)) +
                  (((packet->head.payload.line[c].line_number - 1) & 0x7FFF) *
                   (RtpDepayloader::ingress_.width * kColourspaceBytes.at(ingress_.encoding)));
          length = packet->head.payload.line[c].length & 0xFFFF;

          memcpy(&RtpvrawDepayloader::buffer_in_[pixel], &stream->udpdata[os], length);

          last_packet += length;
          payload += length;
        }

        marker ? receiving = false : receiving = true;

        scan_count = 0;
      }
    }

    stream->arg_tx.encoded_frame = RtpvrawDepayloader::buffer_in_.data();
    new_rx_frame_ = true;
    receiving = true;
  }  // Receive loop
  return;
}

void RtpvrawDepayloader::Start() {
  rx_thread_running_ = true;
  rx_thread_ = std::thread(&RtpvrawDepayloader::ReceiveThread, this);
}

void RtpvrawDepayloader::Stop() {
  rx_thread_running_ = false;

  if (rx_thread_.joinable()) {
    rx_thread_.join();
  }
}

bool RtpvrawDepayloader::WaitForFrame(uint8_t **cpu, int32_t timeout) const {
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
        // Leave the thread to receive the rest of the frame
        return false;
      }
    }
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  }
}

bool RtpvrawDepayloader::Receive(uint8_t **cpu, int32_t timeout [[maybe_unused]]) const {
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
