
# Set Qt6 package
find_package(Qt6 COMPONENTS Core REQUIRED)
# Gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

include_directories(${CMAKE_SOURCE_DIR}/src Qt6::Core ${GST_INCLUDE_DIRS})

set(SOURCES
#   QtSapAnnouncer.cc
  QtSapListener.cc
  QtRtpUncompressedDepayloader.cc
  QtRtpUncompressedPayloader.cc
  QtRtpH264Payloader.cc
  QtRtpH264Depayloader.cc
)

add_library(qtwrapper STATIC ${SOURCES})
target_link_libraries(qtwrapper Qt6::Core)

# Install qtwrapper
install(TARGETS qtwrapper DESTINATION lib)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/qtwrapper)