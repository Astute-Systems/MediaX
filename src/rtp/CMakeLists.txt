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
set_source_files_properties(rtp_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_payloader TYPE SHARED LANGUAGE python SOURCES rtp_payloader.i rtp_payloader.cc ../rtp/rtp_payloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_payloader glog swscale)

# build swig interface for depayloader
set_source_files_properties(rtp_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtp_depayloader TYPE SHARED LANGUAGE python SOURCES rtp_depayloader.i rtp_depayloader.cc ../rtp/rtp_depayloader.cc ../rtp/rtp_utils.cc ../utils/colourspace_cpu.cc)
swig_link_libraries(rtp_depayloader glog swscale)

install(TARGETS rtp_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtp_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtp_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)