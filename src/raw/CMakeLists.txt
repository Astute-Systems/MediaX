project(python)

# find and build swig interface for python
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src)

# build swig interface for payloader
set_source_files_properties(rtpvraw_payloader.i PROPERTIES CPLUSPLUS ON)
swig_add_module(rtpvraw_payloader python rtpvraw_payloader.i)
swig_link_libraries(rtpvraw_payloader ${PYTHON_LIBRARIES})
add_library(rtpvraw_payloader SHARED rtpvraw_payloader.cc)
# build python package
add_custom_command(
    TARGET _rtpvraw_payloader POST_BUILD
    # make python directory if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/rtpvraw_payloader
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_payloader.py ${CMAKE_BINARY_DIR}/python/rtpvraw_payloader
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librtpvraw_payloader.so ${CMAKE_BINARY_DIR}/python/rtpvraw_payloader
)
add_dependencies(_rtpvraw_payloader rtpvraw_payloader )

# build swig interface for depayloader
set_source_files_properties(rtpvraw_depayloader.i PROPERTIES CPLUSPLUS ON)
swig_add_module(rtpvraw_depayloader python rtpvraw_depayloader.i)
swig_link_libraries(rtpvraw_depayloader ${PYTHON_LIBRARIES})
add_library(rtpvraw_depayloader SHARED rtpvraw_depayloader.cc)
# build python package
add_custom_command(
    TARGET _rtpvraw_depayloader POST_BUILD
    # make python directory if it doesn't exist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/rtpvraw_depayloader
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rtpvraw_depayloader.py ${CMAKE_BINARY_DIR}/python/rtpvraw_depayloader
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librtpvraw_depayloader.so ${CMAKE_BINARY_DIR}/python/rtpvraw_depayloader
)
add_dependencies(_rtpvraw_depayloader rtpvraw_depayloader)