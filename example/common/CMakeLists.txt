include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(example_helpers STATIC example_helpers.cc pngget.cc)
target_include_directories(example_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})