cmake_minimum_required(VERSION 3.10)

project(cpp-tools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(includes)

# 扫描src目录内所有cpp文件
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

# 忽略src/tests目录的文件
file(GLOB_RECURSE CPP_SOURCES_EXCLUDES src/tests/*)
list(REMOVE_ITEM CPP_SOURCES ${CPP_SOURCES_EXCLUDES})

message(STATUS "RECURSE FILE IS:" ${CPP_SOURCES})

# 添加三方库
add_subdirectory(third_party)

# 添加单元测试
enable_testing()

add_executable(cpp-tools main.cpp ${CPP_SOURCES})

add_executable(json-test src/tests/json_test.cpp ${CPP_SOURCES})

target_link_libraries(json-test gtest gtest_main)
# add_library(cpp_tools STATIC ${CPP_SOURCES})