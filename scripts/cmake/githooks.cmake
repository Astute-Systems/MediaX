# Setup git hooks

# if no aleady sym linked
execute_process (
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push-check-doxygen ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push-check-doxygen
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push-check-linter ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push-check-linter
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push-check-todo ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push-check-todo
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push-check-untracked ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push-check-untracked
    COMMAND cp -f ${CMAKE_SOURCE_DIR}/.githooks/pre-push-conflict-check ${CMAKE_SOURCE_DIR}/.git/hooks/pre-push-conflict-check
    COMMAND git config core.hooksPath .git/hooks
    OUTPUT_VARIABLE outVar

)
