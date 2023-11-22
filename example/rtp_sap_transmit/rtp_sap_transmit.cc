//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A SAP/SDP listener example
///
/// \file rtp_sap_transmit.cc
///

#include "rtp/rtp.h"

int main(int argc, char *argv[]) {
  mediax::RtpSapTransmit<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader> rtp("238.192.1.1", 5004, "test", 640,
                                                                                      480, 30, "H264");
  std::vector<uint8_t> data;
  data.resize(640 * 480 * 3);
  // Fill the buffer white
  std::fill(data.begin(), data.end(), 255);
  while (true) {
    rtp.Transmit(data.data(), false);
  }
}