find_package(gflags REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/include)

# recieve
add_executable(rtp-sap-receive rtp_sap_receive.cc)
target_link_libraries(rtp-sap-receive mediax_static ${GFLAGS_LIBRARIES} ${GST_LIBRARIES} )

