option(BUILD_CUDA OFF)

project(mediax 
  LANGUAGES CXX
  HOMEPAGE_URL https://github.com/DefenceX/MediaX
  DESCRIPTION "MediaX Library"
  VERSION "${MEDIAX_VERSION}"
)

add_subdirectory(uncompressed)
add_subdirectory(sap)
add_subdirectory(rtp)
add_subdirectory(h264)
add_subdirectory(av1)

if (BUILD_QT6)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)
  add_subdirectory(qt6)
  install(TARGETS mediaxqt DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (BUILD_CUDA)
  enable_language(CUDA)
else()
endif()

include(FindPkgConfig)
include(GNUInstallDirs)

# find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(SWSCALE REQUIRED libswscale)

if (${CMAKE_SYSTEM_NAME} MATCHES "MSYS")
  message("Detected MSYS2...")
  set(MSYS_LIBS ws2_32 mingwex)
endif()

set(SOURCES
  rtp/rtp_utils.cc 
  rtp/rtp_payloader.cc 
  rtp/rtp_depayloader.cc 
  uncompressed/rtp_uncompressed_payloader.cc 
  uncompressed/rtp_uncompressed_depayloader.cc 
  sap/sap_announcer.cc 
  sap/sap_listener.cc 
  sap/sap_utils.cc 
  utils/colourspace_cpu.cc 
  utils/colourspace_cuda.cu
  v4l2/v4l2_source.cc
)

if (GST_SUPPORTED)
  message(STATUS "Adding GST support")
    list(APPEND SOURCES
    h264/gst/open/rtp_h264_payloader.cc 
    h264/gst/open/rtp_h264_depayloader.cc    
    h264/gst/x264/rtp_h264_payloader.cc 
    h264/gst/omx/rtp_h264_payloader.cc 
    h264/gst/omx/rtp_h264_depayloader.cc    
    h264/gst/vaapi/rtp_h264_payloader.cc 
    h264/gst/vaapi/rtp_h264_depayloader.cc
    h265/gst/vaapi/rtp_h265_payloader.cc 
    h265/gst/vaapi/rtp_h265_depayloader.cc
    jpeg/gst/vaapi/rtp_jpeg_payloader.cc 
    jpeg/gst/vaapi/rtp_jpeg_depayloader.cc
    h264/gst/nvidia/rtp_h264_payloader.cc 
    h264/gst/nvidia/rtp_h264_depayloader.cc
    h265/gst/rtp_h265_payloader.cc 
    # h265/gst/rtp_h265_depayloader.cc
    jpeg/gst/rtp_jpeg_payloader.cc 
    jpeg/gst/rtp_jpeg_depayloader.cc
    av1/gst/rtp_av1_payloader.cc 
    av1/gst/rtp_av1_depayloader.cc
  )
endif()

if (VAAPI_SUPPORTED)
  # Use Intel Video Acceleration API (VAAPI) for hardware accelerated video decoding
  list(APPEND SOURCES
    h264/vaapi/rtp_h264_depayloader.cc
    h264/vaapi/rtp_h264_payloader.cc
  )
endif()

if (NVENC_SUPPORTED)
  # Use Nvidia Encoder (NVENC) for hardware accelerated video decoding
  list(APPEND SOURCES
    h264/nvenc/rtp_h264_depayloader.cc
    h264/nvenc/rtp_h264_payloader.cc
  )
endif()

if (!GST_SUPPORTED AND !VAAPI_SUPPORTED AND !NVENC_SUPPORTED)
  # Use native video decoding for software x264 decoding
  list(APPEND SOURCES
    h264/native/rtp_h264_depayloader.cc
    h264/native/rtp_h264_payloader.cc
  )
endif()

include_directories(${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS} ${GST_INCLUDE_DIRS} ${Qt6_INCLUDE_DIRS})
add_library(mediax SHARED ${SOURCES})
if (WIN32)
  target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} wsock32 ws2_32 )
else()
  target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
endif()

set_target_properties(mediax PROPERTIES SOVERSION 1)
set_target_properties(mediax PROPERTIES CUDA_ARCHITECTURES "75")
target_include_directories(mediax PUBLIC ${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS} ${GST_INCLUDE_DIRS})

add_library(mediax_static STATIC ${SOURCES})
if (WIN32)
  target_link_libraries(mediax_static png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} ${GST_LIBRARIES} ${GLOG_LIBRARIES} wsock32 ws2_32)
else()
  target_link_libraries(mediax_static png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} ${GST_LIBRARIES} ${GLOG_LIBRARIES})
endif()

target_include_directories(mediax_static PUBLIC ${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS} ${GST_INCLUDE_DIRS})
set_target_properties(mediax_static PROPERTIES SOVERSION 1)
# set_property(TARGET mediax_static PROPERTY CUDA_SEPARABLE_COMPILATION ON)
# set_target_properties(mediax_static PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(mediax_static PROPERTIES CUDA_ARCHITECTURES "75")

configure_file(${CMAKE_SOURCE_DIR}/scripts/cmake/mediax.pc.in ${CMAKE_BINARY_DIR}/mediax.pc @ONLY) 


