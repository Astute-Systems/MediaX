project(qt-sap-listener)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/qt6 ${CMAKE_BINARY_DIR}/include)

# Qt6 Auto mock
set(CMAKE_AUTOMOC ON)

add_executable(${PROJECT_NAME} sap_listener.cpp sap_receive.cpp)
target_link_libraries(${PROJECT_NAME} mediaxqt)