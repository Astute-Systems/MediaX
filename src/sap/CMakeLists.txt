project(python)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src)

# build swig interface for depayloader
set_source_files_properties(sap_announcer.i PROPERTIES CPLUSPLUS ON)
swig_add_module(sap_announcer python sap_announcer.i)
swig_link_libraries(sap_announcer ${PYTHON_LIBRARIES})
add_library(sap_announcer SHARED sap_announcer.cc)
# build python package
add_custom_command(
    TARGET _sap_announcer POST_BUILD
    # make python directory if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/sap_announcer
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/sap_announcer.py ${CMAKE_BINARY_DIR}/python/sap_announcer
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsap_announcer.so ${CMAKE_BINARY_DIR}/python/sap_announcer
)
add_dependencies(_sap_announcer sap_announcer)

# Create a ptyhon object just for this package
set_source_files_properties(sap_listener.i PROPERTIES CPLUSPLUS ON)
swig_add_module(sap_listener python sap_listener.i)
swig_link_libraries(sap_listener ${PYTHON_LIBRARIES})
add_library(sap_listener SHARED sap_listener.cc)
# build python package
add_custom_command(
    TARGET _sap_listener POST_BUILD
    # make python directory if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/sap_listener
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/sap_listener.py ${CMAKE_BINARY_DIR}/python/sap_listener
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsap_listener.so ${CMAKE_BINARY_DIR}/python/sap_listener
)

add_dependencies(_sap_listener sap_listener)