cmake_minimum_required(VERSION 3.20)
project(bpm-finder)
set(CMAKE_CXX_STANDARD 20)
include_directories(src)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(bpm-finder ${SRC_FILES})

# ----- Add Google Test as a Dependency -----
cmake_policy(SET CMP0135 NEW)
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0  # Use Git tag instead of ZIP download
        GIT_SHALLOW TRUE  # Only fetch the specific commit for faster download
)

# Makes gtest targets (like GTest::gtest_main) available
FetchContent_MakeAvailable(googletest)

# Enable testing for CTest integration
enable_testing()

add_subdirectory(tests)

# ----- Add libraries to link against for WASAPI audio capture -----
target_link_libraries(bpm-finder
        ole32
        uuid
        avrt
        oleaut32
)
