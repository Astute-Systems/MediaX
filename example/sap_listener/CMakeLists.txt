include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(sap-listener sap-listener.cc)
target_link_libraries(sap-listener mediax_static)