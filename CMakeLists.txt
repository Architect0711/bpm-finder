cmake_minimum_required(VERSION 3.20)
project(bpm-finder)
set(CMAKE_CXX_STANDARD 20)

include_directories(src)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(bpm-finder ${SRC_FILES})
