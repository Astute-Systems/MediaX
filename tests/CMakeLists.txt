include(FindPkgConfig)

# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

include_directories(${CMAKE_SOURCE_DIR}/src  ${GST_INCLUDE_DIRS})

add_executable(
  tests
  colourspace_tests.cc
  rtp_utils_tests.cc
  rtp_payloader_tests.cc
  sap_announcer_tests.cc
  sap_listener_tests.cc
)

target_link_libraries(tests
  gtest
  gtest_main 
  mediax_static
  ${GST_LIBRARIES}
)

target_include_directories(
  tests PUBLIC
  ${TEST_INCLUDES}
)

include(GoogleTest)

gtest_discover_tests(tests)

