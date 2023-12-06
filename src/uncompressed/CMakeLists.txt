project(python)

cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

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
include_directories(${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src ${GST_INCLUDE_DIRS})

# add a CFLAG for SWIG
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSWIG")

# build swig interface for payloader
set_source_files_properties(rtp_uncompressed_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_uncompressed_payloader TYPE SHARED LANGUAGE python SOURCES rtp_uncompressed_payloader.i rtp_uncompressed_payloader.cc ../rtp/rtp_payloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
set_target_properties(rtp_uncompressed_payloader PROPERTIES VERSION ${MEDIAX_VERSION} SOVERSION ${MEDIAX_MAJOR_VERSION})
swig_link_libraries(rtp_uncompressed_payloader glog swscale)

# build swig interface for depayloader
set_source_files_properties(rtp_uncompressed_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_uncompressed_depayloader TYPE SHARED LANGUAGE python SOURCES rtp_uncompressed_depayloader.i rtp_uncompressed_depayloader.cc ../rtp/rtp_depayloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_uncompressed_depayloader glog swscale)
set_target_properties(rtp_uncompressed_depayloader PROPERTIES VERSION 1.0.0)

install(TARGETS rtp_uncompressed_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_uncompressed_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtp_uncompressed_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_uncompressed_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)