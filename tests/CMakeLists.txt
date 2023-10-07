include(FindPkgConfig)

# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(SWSCALE REQUIRED libswscale libavutil) # libavformat libavcodec

set(MEDIAX_LIBRARIES ${EXTERNAL_PREFIX}/usr/local/lib/libmediax_static.a ${GST_LIBRARIES})

link_directories(${CAIRO_LIBRARY_DIRS})
add_executable(
  tests
  colourspace_tests.cc
  util_tests.cc
  testcard_tests.cc
  rtp_tests.cc
  rtp_utils_tests.cc
  rtp_raw_payloader_tests.cc
  rtp_raw_depayloader_tests.cc
  rtp_h264_gst_depayloader_tests.cc
  sap_announcer_tests.cc
  sap_listener_tests.cc
  sap_utils_tests.cc
  main.cc
)

target_link_libraries(tests
  gtest
  mediax_static
  ${GST_LIBRARIES}
  ${SWSCALE_LIBRARIES}
  pthread
  glog
  gflags
)

target_include_directories(
  tests PUBLIC
  ${TEST_INCLUDES}
)

include(GoogleTest)

gtest_discover_tests(tests)

