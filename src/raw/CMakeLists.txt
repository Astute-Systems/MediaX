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
swig_add_library(rtpvraw_payloader TYPE SHARED LANGUAGE python SOURCES rtpvraw_payloader.i)

# build swig interface for depayloader
set_source_files_properties(rtpvraw_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_library(rtpvraw_depayloader TYPE SHARED LANGUAGE python SOURCES rtpvraw_depayloader.i)

install(TARGETS rtpvraw_payloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_payloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS rtpvraw_depayloader DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_depayloader.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)