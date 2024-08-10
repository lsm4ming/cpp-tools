cmake_minimum_required(VERSION 3.10)

project(cpptools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# 创建 INTERFACE 目标
add_library(cpptools_include INTERFACE)
target_include_directories(cpptools_include INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# 创建实际的库目标
add_library(cpptools STATIC ${CPP_SOURCES})

# 链接 INTERFACE 目标到实际的库目标
target_link_libraries(cpptools PUBLIC cpptools_include)

# 设置 cpp-tools 目标的包含目录
target_include_directories(cpptools PUBLIC includes)