# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

include_directories(${CMAKE_SOURCE_DIR}/src ${GST_INCLUDE_DIRS})

add_library(h264 STATIC rtp_h264_payloader.cc rtp_h264_depayloader.cc)
target_link_libraries(h264 ${GST_LIBRARIES})
target_include_directories(h264 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})