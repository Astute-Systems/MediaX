
# Set Qt6 package
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
# Gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

# Qt auto
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR}/src Qt6::Core ${GST_INCLUDE_DIRS})

set(SOURCES
  QtSapAnnouncer.cc
  QtSapListener.cc
  QtRtpUncompressedDepayloader.cc
  QtRtpUncompressedPayloader.cc
  QtRtpH264Payloader.cc
  QtRtpH264Depayloader.cc
)

add_library(mediaxqt STATIC ${SOURCES})
target_include_directories(mediaxqt PUBLIC ${CMAKE_SOURCE_DIR}/src t Qt6::Core ${GST_INCLUDE_DIRS})
target_link_libraries(mediaxqt mediax_static Qt6::Core Qt::Widgets)

# Install qtwrapper
install(TARGETS mediaxqt DESTINATION lib)
