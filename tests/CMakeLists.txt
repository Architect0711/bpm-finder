# Create test executable
file(GLOB_RECURSE TEST_FILES *.cpp)
add_executable(bpm-finder-tests ${TEST_FILES})

# Link against Google Test AND the main library
target_link_libraries(bpm-finder-tests
        GTest::gtest_main
        GTest::gtest
        bpm-finder-lib
)

# Add test to CTest
add_test(NAME bpm-finder-tests COMMAND bpm-finder-tests)
