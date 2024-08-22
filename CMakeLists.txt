cmake_minimum_required(VERSION 3.10)

project(cpptools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 扫描src目录内所有cpp文件
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

# 忽略src/tests目录的文件
file(GLOB_RECURSE CPP_SOURCES_EXCLUDES src/tests/*)
list(REMOVE_ITEM CPP_SOURCES ${CPP_SOURCES_EXCLUDES})

# 添加三方库
add_subdirectory(third_party)

include_directories(includes)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "debug模式构建测试代码...")
    add_executable(cpptools_exec main.cpp ${CPP_SOURCES})

    add_executable(http src/tests/http.cpp ${CPP_SOURCES})

    add_executable(generator src/tests/generator.cpp ${CPP_SOURCES})

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_executable(reactor reactor.cpp src/common/system.cpp)

        add_executable(echo src/tests/echo.cpp)
    endif ()
endif ()

# 创建静态库目标
add_library(cpptools STATIC ${CPP_SOURCES})

# 设置 cpptools 目标的包含目录
target_include_directories(cpptools PUBLIC includes)