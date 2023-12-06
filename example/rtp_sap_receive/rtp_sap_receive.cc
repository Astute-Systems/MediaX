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
/// \file rtp_sap_reciever.cc
///

// [Receive example wrapper]
#include "rtp/rtp.h"
int main(int argc, char *argv[]) {
  mediax::RtpSapRecieve<mediax::rtp::uncompressed::RtpUncompressedDepayloader> rtp(
      "238.192.1.1", 5004, "test-session-name", 640, 480, 25, "RGB24");
  ::mediax::rtp::RtpFrameData data;
  for (int count = 0; count < 100; count++) rtp.Receive(&data, false);
}
// [Receive example wrapper]