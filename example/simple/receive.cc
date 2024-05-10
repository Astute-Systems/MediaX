//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file receive.cc
///

// [Receive includes]
#include "rtp/rtp.h"
#include "sap/sap.h"
// [Receive includes]

///
/// \brief Dummy function for example only
///
///
void NvidiaH264() {
  // [Receive example nvidia]
  // To use another payloader, simply change the namespace i.e. Nvidia Video Codec SDK
  mediax::rtp::h264::gst::nvidia::RtpH264GstNvidiaPayloader rtp;
  // [Receive example nvidia]
}

///
/// \brief Dummy function for example only
///
///
void VaapiH264() {
  // [Receive example vaapi]
  // To use another payloader, simply change the namespace i.e. Intel Video Accelleration API (VAAPI)
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader rtp;
  // [Receive example vaapi]
}

///
/// \brief Dummy function for example only
///
///
void VaapiH265() {
  // [Receive example vaapi h265]
  // To use another payloader, simply change the namespace i.e. Intel Video Accelleration API (VAAPI)
  mediax::rtp::h265::gst::vaapi::RtpH265GstVaapiPayloader rtp;
  // [Receive example vaapi h265]
}

int main(int argc, char *argv[]) {
  // [Receive example sap]
  mediax::rtp::StreamInformation stream_information_via_sap;

  // Get the SAP/SDP listener singleton instance
  ::mediax::sap::SapListener &sap = ::mediax::sap::SapListener::GetInstance();
  // You can use the SAP callback here instead of polling
  sap.RegisterSapListener(
      "session_test",
      [](const ::mediax::sap::SdpMessage *sdp_message, void *user_data [[maybe_unused]]) {
        std::cout << "Got SAP callback\n";
        std::cout << "  Session name: " << sdp_message->session_name << "\n";
      },
      nullptr);
  // Wait 1.1 seconds as the SAP announcement is sent every second and we need a sampling period
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  // Check for announcments
  if (const std::map<std::string, mediax::sap::SdpMessage, std::less<>> &announcements = sap.GetSapAnnouncements();
      announcements.empty()) {
    std::cout << "No SAP/SDP announcements seen\n";
    return 0;
  } else {
    stream_information_via_sap = ::mediax::sap::SapToStreamInformation(announcements.at("session_test"));
  }
  // [Receive example sap]

  // [Receive example open]
  // Initalise the RTP library once on startup
  mediax::InitRtp(argc, argv);
  // Setup RTP streaming class for receive
  mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;
  // Create a stream information object
  mediax::rtp::StreamInformation stream_information = {
      "session_test", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  // A recieve buffer sized to our needs
  std::vector<uint8_t> cpu_buffer(stream_information.width * stream_information.height *
                                  ::mediax::BytesPerPixel(stream_information.encoding));
  // Create the RTP stream
  rtp.SetStreamInfo(stream_information);
  // [Receive example open]

  // [Receive example receive]
  // Register a callback to handle the received video
  rtp.RegisterCallback([](const mediax::rtp::RtpDepayloader &depay [[maybe_unused]], mediax::rtp::RtpFrameData frame) {
    // Do something with the frame
    const uint8_t *data = frame.cpu_buffer;
    if (data != nullptr) {
      std::cerr << "Received data, do something with it here\n";
    } else {
      std::cerr << "Timedout\n";
    }
  });
  rtp.Start();
  // [Receive example receive]

  // [Receive example close]
  // Close the RTP session
  rtp.Stop();
  rtp.Close();
  mediax::RtpCleanup();
  // [Receive example close]

  return 0;
}
