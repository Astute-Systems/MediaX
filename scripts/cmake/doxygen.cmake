add_custom_target(doxygen)    

add_custom_command(
    TARGET doxygen
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/images
    COMMAND cd ${CMAKE_BINARY_DIR} && cmake .. --graphviz=depends.dot && dot -Tpng  depends.dot.mediax -o ${CMAKE_BINARY_DIR}/images/mediax.png
    # COMMAND cd ${CMAKE_BINARY_DIR} && dot -Tpng  depends.dot.tests -o ${CMAKE_BINARY_DIR}/images/test_depends.png
    COMMAND GIT_HASH=${GIT_HASH} CMAKE_MAJOR_VERSION=${CMAKE_MAJOR_VERSION} CMAKE_MINOR_VERSION=${CMAKE_MINOR_VERSION} CMAKE_PATCH_VERSION=${CMAKE_PATCH_VERSION} CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR} doxygen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})