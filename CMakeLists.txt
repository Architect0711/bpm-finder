cmake_minimum_required(VERSION 3.20)
project(bpm-finder)
set(CMAKE_CXX_STANDARD 20)
include_directories(src)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(bpm-finder ${SRC_FILES})

# libraries to link against for WASAPI audio capture
target_link_libraries(bpm-finder
        ole32
        uuid
        avrt
        oleaut32
)
