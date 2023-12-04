project(qt-sap-announcer)


# Create an executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link any libraries here
target_link_libraries(${PROJECT_NAME} 
    mediaxqt
)

add_executable(${PROJECT_NAME} cer sap_announcer.cpp)
target_link_libraries(${PROJECT_NAME}  mediaxqt)