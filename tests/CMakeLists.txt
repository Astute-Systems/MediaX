enable_testing()

include(FindPkgConfig)

set(CMAKE_CXX_FLAGS --coverage)

set(
  TEST_INCLUDES
  ${CMAKE_SOURCE_DIR}/src
)

add_executable(
  tests
  colourspace.cc
)


target_link_libraries(tests
  gtest
  gtest_main 
  mediax_static
)

target_include_directories(
  tests PUBLIC
  ${TEST_INCLUDES}
)

include(GoogleTest)
