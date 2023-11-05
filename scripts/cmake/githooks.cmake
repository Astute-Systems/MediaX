# Setup git hooks

# if no aleady sym linked
execute_process (
    COMMAND rm -rf ${CMAKE_SOURCE_DIR}/.git/hooks
    COMMAND ln -s ${CMAKE_SOURCE_DIR}/.githooks ${CMAKE_SOURCE_DIR}/.git/hooks
    OUTPUT_VARIABLE outVar

)
message (STATUS "Symlinked git hooks")