project(python)

cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src)

# build swig interface for payloader
set_source_files_properties(rtpvraw_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtpvraw_payloader TYPE SHARED LANGUAGE python SOURCES rtpvraw_payloader.i rtpvraw_payloader.cc ../rtp/rtp_payloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
set_target_properties(rtpvraw_payloader PROPERTIES VERSION ${MEDIAX_VERSION} SOVERSION ${MEDIAX_MAJOR_VERSION})
swig_link_libraries(rtpvraw_payloader glog swscale)

# build swig interface for depayloader
set_source_files_properties(rtpvraw_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtpvraw_depayloader TYPE SHARED LANGUAGE python SOURCES rtpvraw_depayloader.i rtpvraw_depayloader.cc ../rtp/rtp_depayloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
set_target_properties(rtpvraw_depayloader PROPERTIES VERSION ${MEDIAX_VERSION} SOVERSION ${MEDIAX_MAJOR_VERSION})
swig_link_libraries(rtpvraw_depayloader glog swscale)

install(TARGETS rtpvraw_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtpvraw_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)