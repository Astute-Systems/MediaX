include(FindPkgConfig)

set(CMAKE_CXX_FLAGS --coverage)

set(
  TEST_INCLUDES
  ${CMAKE_SOURCE_DIR}/src
)

include_directories(${TEST_INCLUDES})

add_executable(
  tests
  colourspace_tests.cc
  rtp_utils_tests.cc
  rtp_payloader_tests.cc
  rtp_depayloader_tests.cc
  sap_announcer_tests.cc
  sap_listener_tests.cc
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

# define tests
add_test(NAME hello COMMAND echo "Hello World")

gtest_discover_tests(tests)

