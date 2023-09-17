option(BUILD_CUDA OFF)

project(mediax 
  LANGUAGES CXX
  HOMEPAGE_URL https://github.com/DefenceX/MediaX
  DESCRIPTION "MediaX Library"
  VERSION "${MEDIAX_VERSION}"
)

if (BUILD_CUDA)
  enable_language(CUDA)
else()
endif()

include(FindPkgConfig)
include(GNUInstallDirs)

# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(SWSCALE REQUIRED libswscale)

if (${CMAKE_SYSTEM_NAME} MATCHES "MSYS")
  message("Detected MSYS2...")
  set(MSYS_LIBS ws2_32 mingwex)
endif()

set(SOURCES
  rtp/rtp_utils.cc 
  rtp/rtp_payloader.cc 
  rtp/rtp_depayloader.cc 
  raw/rtpvraw_payloader.cc 
  raw/rtpvraw_depayloader.cc 
  sap/sap_announcer.cc 
  sap/sap_listener.cc 
  sap/sap_utils.cc 
  utils/colourspace_cpu.cc 
  utils/colourspace_cuda.cu
  v4l2/v4l2_source.cc
  h264/vaapi/rtph264_payloader.cc 
  h264/vaapi/rtph264_depayloader.cc
)
option(GST_SUPPORTED "GStreamer support" ON)
if (GST_SUPPORTED)
  message(STATUS "Adding GST support")
    list(APPEND SOURCES
    h264/gst/rtph264_payloader.cc 
    h264/gst/rtph264_depayloader.cc
  )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGST_SUPPORTED")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(mediax SHARED ${SOURCES})

set_target_properties(mediax PROPERTIES SOVERSION 1)
set_target_properties(mediax PROPERTIES CUDA_ARCHITECTURES "75")
target_include_directories(mediax PUBLIC ${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS} ${GST_INCLUDE_DIRS})


add_library(mediax_static STATIC ${SOURCES})
if (WIN32)
  target_link_libraries(mediax_static png glog pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} ${GST_LIBRARIES} wsock32 ws2_32)
else()
  target_link_libraries(mediax_static png glog pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} ${GST_LIBRARIES})
endif()

target_include_directories(mediax_static PUBLIC ${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS} ${GST_INCLUDE_DIRS})
set_target_properties(mediax_static PROPERTIES SOVERSION 1)
# set_property(TARGET mediax_static PROPERTY CUDA_SEPARABLE_COMPILATION ON)
# set_target_properties(mediax_static PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(mediax_static PROPERTIES CUDA_ARCHITECTURES "75")

configure_file(${CMAKE_SOURCE_DIR}/scripts/cmake/mediax.pc.in ${CMAKE_BINARY_DIR}/mediax.pc @ONLY) 


