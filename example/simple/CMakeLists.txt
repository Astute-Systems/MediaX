
include_directories(${CMAKE_SOURCE_DIR}/src)
# GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0)

add_executable(simple-receive receive.cc)
target_link_libraries(simple-receive mediax ${GST_LIBRARIES})

add_executable(simple-transmit transmit.cc)
target_link_libraries(simple-transmit mediax ${GST_LIBRARIES})

