# Create test executable
file(GLOB_RECURSE TEST_FILES *.cpp)
add_executable(bpm-finder-tests ${TEST_FILES})

# Link against Google Test
target_link_libraries(bpm-finder-tests
        GTest::gtest_main
        GTest::gtest
)

# Add test to CTest
add_test(NAME bpm-finder-tests COMMAND bpm-finder-tests)