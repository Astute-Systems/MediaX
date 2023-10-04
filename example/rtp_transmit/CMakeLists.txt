find_package(gflags REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/include)

add_executable(rtp-transmit transmit-example.cc)
target_link_libraries(rtp-transmit example_helpers mediax_static ${GFLAGS_LIBRARIES} ${GST_LIBRARIES} )
