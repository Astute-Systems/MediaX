#include <gmock/gmock.h>

#include "rtp/rtp_payloader.h"

namespace mediax::rtp {

class MockRtpPayloader : public mediax::rtp::RtpPayloader {
 public:
  MOCK_METHOD(void, SetStreamInfo, (const ::mediax::rtp::StreamInformation& stream_information), (override));
  MOCK_METHOD(bool, Open, (), ());
  MOCK_METHOD(void, Start, (), ());
  MOCK_METHOD(void, Stop, (), ());
  MOCK_METHOD(void, Close, (), ());
  MOCK_METHOD(int, Transmit, (uint8_t * rgbframe, bool blocking), (override));
  MOCK_METHOD(void, SetSessionName, (std::string_view name), ());
  MOCK_METHOD(std::string, GetSessionName, (), (const));
  MOCK_METHOD(::mediax::rtp::ColourspaceType, GetColourSpace, (), (const));
  MOCK_METHOD(void, SetHeight, (uint32_t height), ());
  MOCK_METHOD(uint32_t, GetHeight, (), (const));
  MOCK_METHOD(void, SetWidth, (uint32_t width), ());
  MOCK_METHOD(uint32_t, GetWidth, (), (const));
  MOCK_METHOD(void, SetFramerate, (uint32_t framerate), ());
  MOCK_METHOD(uint32_t, GetFrameRate, (), (const));
  MOCK_METHOD(std::string, GetIpAddress, (), (const));
  MOCK_METHOD(void, SetIpAddress, (std::string_view ip_address), ());
  MOCK_METHOD(void, SetPort, (uint32_t port), ());
  MOCK_METHOD(uint32_t, GetPort, (), (const));
};

}  // namespace mediax::rtp
