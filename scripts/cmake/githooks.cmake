# Setup git hooks

execute_process (
    COMMAND git config core.hooksPath ${CMAKE_SOURCE_DIR}/.githooks
    OUTPUT_VARIABLE outVar
)