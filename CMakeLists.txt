cmake_minimum_required(VERSION 3.20)
project(bpm-finder)
set(CMAKE_CXX_STANDARD 20)
include_directories(src)

# Separate main.cpp from library sources
file(GLOB_RECURSE LIB_FILES src/*.cpp)
list(FILTER LIB_FILES EXCLUDE REGEX ".*main\\.cpp$")

# Create a library from all source files except main.cpp
add_library(bpm-finder-lib ${LIB_FILES})

# Create the main executable
add_executable(bpm-finder src/main.cpp)
target_link_libraries(bpm-finder bpm-finder-lib)

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


# ----- Add spdlog as a Dependency -----
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1  # Latest stable version
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)

# Link spdlog to the library
target_link_libraries(bpm-finder-lib PUBLIC spdlog::spdlog)

# Enable testing for CTest integration
enable_testing()

add_subdirectory(tests)

# ----- Add libraries to link against for WASAPI audio capture -----
target_link_libraries(bpm-finder-lib PUBLIC
        ole32
        uuid
        avrt
        oleaut32
)
