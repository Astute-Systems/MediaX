project(python)

cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSWIG")

# Gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

# if CMAKE > 3.27
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.27.0")
    find_package(Python COMPONENTS Interpreter Development)
    include_directories(${Python_INCLUDE_DIRS})
else()
    find_package(PythonInterp REQUIRED)
    find_package(PythonLibs REQUIRED)
endif()
include_directories(${PYTHON_INCLUDE_PATH} ${GST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)

# build swig interface for payloader
set_source_files_properties(rtp_h264_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_h264_gst_nvidia_payloader TYPE SHARED LANGUAGE python SOURCES rtp_h264_payloader.i ../../../rtp/rtp_payloader.cc ../../../rtp/rtp_payloader.cc ../../../rtp/rtp_utils.cc ../../../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_h264_gst_nvidia_payloader swscale)

# build swig interface for depayloader
set_source_files_properties(rtp_h264_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_h264_gst_nvidia_depayloader TYPE SHARED LANGUAGE python SOURCES rtp_h264_depayloader.i ${CMAKE_SOURCE_DIR}/src/rtp/rtp_depayloader.cc ${CMAKE_SOURCE_DIR}/src/rtp/rtp_depayloader.cc ${CMAKE_SOURCE_DIR}/src/rtp/rtp_utils.cc ../../../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_h264_gst_nvidia_depayloader swscale)

install(TARGETS rtp_h264_gst_nvidia_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_h264_gst_nvidia_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtp_h264_gst_nvidia_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_h264_gst_nvidia_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)