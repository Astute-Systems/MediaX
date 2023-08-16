option(BUILD_CUDA OFF)

if (BUILD_CUDA)
  project(mediax-with-cuda LANGUAGES CXX CUDA)
  enable_language(CUDA)

else()
  project(mediax-without-cuda LANGUAGES CXX)
endif()

include(FindPkgConfig)
include(GNUInstallDirs)

if (${CMAKE_SYSTEM_NAME} MATCHES "MSYS")
  message("Detected MSYS2...")
  set(MSYS_LIBS ws2_32 mingwex)
endif()

set(SOURCES
  rtp_payloader.cc 
  rtp_depayloader.cc 
  rtpvraw_payloader.cc 
  rtpvraw_depayloader.cc 

  rtp_utils.cc 
  sap_announcer.cc 
  sap_listener.cc 
  sap_utils.cc 
  colourspace.cc 
  colourspace_cuda.cu
  v4l2_source.cc
)

if (VAAPI) 
  list(APPEND SOURCES   
    rtph264_payloader.cc 
    rtph264_depayloader.cc
  )
endif()

add_library(mediax SHARED ${SOURCES})
pkg_check_modules(SWSCALE REQUIRED libswscale libavutil) # libavformat libavcodec
if (WIN32)
  message(STATUS "Adding windows socket libs")
  target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} wsock32 ws2_32)
else()
  target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
endif()
target_include_directories(mediax PUBLIC ${SWSCALE_INCLUDE_DIRS})
set_target_properties(mediax PROPERTIES SOVERSION 1)
# set_property(TARGET mediax PROPERTY CUDA_SEPARABLE_COMPILATION ON)
# set_target_properties(mediax PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(mediax PROPERTIES CUDA_ARCHITECTURES "75")

add_library(mediax_static STATIC ${SOURCES})
if (WIN32)
  target_link_libraries(mediax_static png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS} wsock32 ws2_32)
else()
  target_link_libraries(mediax_static png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
endif()

target_include_directories(mediax_static PUBLIC ${CMAKE_SOURCE_DIR}/src ${SWSCALE_INCLUDE_DIRS})
set_target_properties(mediax_static PROPERTIES SOVERSION 1)
# set_property(TARGET mediax_static PROPERTY CUDA_SEPARABLE_COMPILATION ON)
# set_target_properties(mediax_static PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(mediax_static PROPERTIES CUDA_ARCHITECTURES "75")


