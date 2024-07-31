include(FetchContent)

# 下载gtest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.x
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 下载boots
#FetchContent_Declare(
#        boots
#        GIT_REPOSITORY https://github.com/boostorg/boost.git
#        GIT_TAG boost-1.85.0
#        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
#)
#FetchContent_MakeAvailable(boots)