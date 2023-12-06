project(qt-rtp-receive)

# Qt6 Auto mock
set(CMAKE_AUTOMOC ON)

# Add your source files here
set(SOURCES
    main.cpp
    rtp_receive.cpp
)

# Add your header files here
set(HEADERS
)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/qt6 ${CMAKE_BINARY_DIR}/include)

# Create an executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link any libraries here
target_link_libraries(${PROJECT_NAME} 
    mediaxqt
)
