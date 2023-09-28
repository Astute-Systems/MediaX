include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(sap-listener sap-listener-example.cc)
target_link_libraries(sap-listener mediax_static)