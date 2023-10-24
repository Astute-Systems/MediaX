# Setup git hooks

if(EXISTS ${CMAKE_SOURCE_DIR}/.githooks)
    execute_process (
        COMMAND git config core.hooksPath ${CMAKE_SOURCE_DIR}/.githooks
        OUTPUT_VARIABLE outVar
    )
endif()