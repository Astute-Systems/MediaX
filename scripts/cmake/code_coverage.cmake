if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(COVERAGE_FLAGS "-g -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")

    add_custom_target(lcov)    
    add_custom_command(
        TARGET lcov
        COMMAND ${CMAKE_BINARY_DIR}/bin/tests --v=1
        COMMAND lcov -c -d . -o main_coverage.info 
        COMMAND lcov -r main_coverage.info '*test*' -o main_coverage_filtered.info
        COMMAND lcov -r main_coverage_filtered.info '*_deps*' -o main_coverage_filtered.info
        COMMAND genhtml main_coverage_filtered.info --output-directory out
        COMMAND echo "Now open ./out/index.html in your browser"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_custom_target(gcov)    
    add_custom_command(
        TARGET gcov
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/*.gcno
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/*.gcov
        COMMAND qemu-arm ${CMAKE_BINARY_DIR}/bin/tests --database_filename=${CMAKE_BINARY_DIR}/database/settings.db --gtest_filter=-*NOPIPELINE* -logtostderr=1 --v=1
        COMMAND find . -name '*.gcda' | xargs gcov
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

else()
    add_custom_target(lcov)
    add_custom_command(
        TARGET lcov
        COMMENT "This target is must be build with -DCMAKE_BUILD_TYPE=Debug"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()