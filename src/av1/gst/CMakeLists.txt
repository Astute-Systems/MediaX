project(python)

cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

# Gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH} ${GST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)

# build swig interface for payloader
set_source_files_properties(rtp_av1_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_av1_payloader TYPE SHARED LANGUAGE python SOURCES rtp_av1_payloader.i ../../rtp/rtp_payloader.cc ../../rtp/rtp_payloader.cc ../../rtp/rtp_utils.cc ../../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_av1_payloader swscale)

# build swig interface for depayloader
set_source_files_properties(rtp_av1_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_av1_depayloader TYPE SHARED LANGUAGE python SOURCES rtp_av1_depayloader.i ${CMAKE_SOURCE_DIR}/src/rtp/rtp_depayloader.cc ${CMAKE_SOURCE_DIR}/src/rtp/rtp_depayloader.cc ${CMAKE_SOURCE_DIR}/src/rtp/rtp_utils.cc ../../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_av1_depayloader swscale)

install(TARGETS rtp_av1_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_av1_gst_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtp_av1_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_av1_gst_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)