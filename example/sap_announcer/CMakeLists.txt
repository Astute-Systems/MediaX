include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(sap-announcer sap-announcer.cc)
target_link_libraries(sap-announcer mediax_static)