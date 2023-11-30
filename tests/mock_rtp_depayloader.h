#include <gmock/gmock.h>

#include "rtp/rtp_depayloader.h"

namespace mediax::rtp {

class MockRtpDepayloader : public mediax::rtp::RtpDepayloader {
 public:
  MOCK_METHOD(void, SetStreamInfo, (const ::mediax::rtp::StreamInformation& stream_information), (override));
  MOCK_METHOD(void, RegisterCallback, (const ::mediax::rtp::RtpCallback& callback), ());
  MOCK_METHOD(bool, Open, (), (override));
  MOCK_METHOD(void, Start, (), (override));
  MOCK_METHOD(void, Stop, (), (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(bool, Receive, (::mediax::rtp::RtpFrameData * data, int32_t timeout), (override));
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
  MOCK_METHOD(void, SetColourSpace, (::mediax::rtp::ColourspaceType colourspace), ());
  MOCK_METHOD(bool, IsMulticast, (std::string_view ip_address), ());
  MOCK_METHOD(bool, CallbackRegistered, (), (const));
  MOCK_METHOD(void, UnregisterCallback, (), ());
  MOCK_METHOD(void, Callback, (::mediax::rtp::RtpFrameData frame), (const, override));
  MOCK_METHOD(::mediax::rtp::RtpPortType&, GetStream, (), ());
  MOCK_METHOD(::mediax::rtp::RtpCallback, GetCallback, (), (const));
  MOCK_METHOD(std::vector<uint8_t>&, GetBuffer, (), ());
};
}  // namespace mediax::rtp