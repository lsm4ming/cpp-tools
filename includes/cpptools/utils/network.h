#pragma once

#include <arpa/inet.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netinet/tcp.h>
#include <cstring>
#include "cpptools/common/common.h"
#if defined(OS_LINUX)
#include <sys/sysinfo.h>
#endif

using namespace cpptools::common;

namespace cpptools::utils
{
    class Network
    {
    public:
        /**
         * 获取本机IP
         * @param ethName
         * @return
         */
        static String getIp(const String &ethName);
    };
}