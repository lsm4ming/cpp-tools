include(FetchContent)

# 下载gtest
FetchContent_Declare(
        googletest
        # GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG v1.14.0
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 下载openssl
#FetchContent_Declare(
#        openssl
#        GIT_REPOSITORY https://github.com/openssl/openssl.git
#        GIT_TAG openssl-3.0.14
#        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
#)
#FetchContent_MakeAvailable(openssl)

# 下载boots
#FetchContent_Declare(
#        boots
#        GIT_REPOSITORY https://github.com/boostorg/boost.git
#        GIT_TAG boost-1.85.0
#        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
#)
#FetchContent_MakeAvailable(boots)