project(python)

cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

# Gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src ${GST_INCLUDE_DIRS})

# build swig interface for depayloader
set_source_files_properties(sap_announcer.i PROPERTIES CPLUSPLUS ON)
swig_add_library(sap_announcer TYPE MODULE LANGUAGE python SOURCES sap_announcer.i sap_announcer.cc sap_utils.cc ../rtp/rtp_utils.cc)
swig_link_libraries(sap_announcer glog)

# Create a ptyhon object just for this package
set_source_files_properties(sap_listener.i PROPERTIES CPLUSPLUS ON)
swig_add_library(sap_listener TYPE MODULE LANGUAGE python SOURCES sap_listener.i sap_listener.cc ../rtp/rtp_utils.cc)
swig_link_libraries(sap_listener glog)

install(TARGETS sap_announcer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sap_announcer.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(TARGETS sap_listener DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sap_listener.py DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages/mediax)