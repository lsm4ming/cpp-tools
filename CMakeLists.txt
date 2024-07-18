cmake_minimum_required(VERSION 3.10)

project(cpp-tools)

set(CMAKE_CXX_STANDARD 20)

include_directories(includes)

# 扫描src目录内所有cpp文件
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

message(STATUS "RECURSE FILE IS:" ${CPP_SOURCES})

add_executable(cpp-tools main.cpp ${CPP_SOURCES})