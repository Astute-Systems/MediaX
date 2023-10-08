include(FindPkgConfig)

# Set Qt6 package
find_package(Qt6 COMPONENTS Core REQUIRED)

# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(SWSCALE REQUIRED libswscale libavutil) # libavformat libavcodec

set(MEDIAX_LIBRARIES ${EXTERNAL_PREFIX}/usr/local/lib/libmediax_static.a ${GST_LIBRARIES} Qt6::Core)

link_directories(Qt6::Core ${CAIRO_LIBRARY_DIRS})

set(SOURCES
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

if (BUILD_QT6)
  list(APPEND SOURCES qt/qt_sap_announcer_tests.cc)
  list(APPEND SOURCES qt/qt_sap_listener_tests.cc)
endif()

add_executable(
  tests
  ${SOURCES}
)

target_link_libraries(tests
  gtest
  mediax
  mediaxqt
  Qt6::Core
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

if (BUILD_QT6)
  add_dependencies(tests mediaxqt)
endif()

include(GoogleTest)

gtest_discover_tests(tests)

