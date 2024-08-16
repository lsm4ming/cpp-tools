#ifndef CPPTOOLS_SYSTEM_H
#define CPPTOOLS_SYSTEM_H

#include <cstdlib>
#include "common.h"
#if defined(OS_LINUX)
#include <sys/sysinfo.h>
#endif

namespace cpptools::common
{
    class System
    {
    public:
        /**
         * 退出系统
         * @param code
         */
        static void exit(int code = -1);

        /**
         * 睡眠
         * @param ms 毫秒
         */
        static void sleep(uint32 ms);

        /**
         * 获取cpu数量
         * @return
         */
        static uint32 cpuNumber();

        /**
         * 获取总内存
         */
        static uint64 getTotalMemory(const String& unit = "B");

        /**
         * 获取当前可用内存
         */
        static uint64 getAvailableMemory(const String& unit = "B");

        /**
         * 获取已用内存
         */
        static uint64 getUsedMemory(const String& unit = "B");
    };
}

#endif //CPPTOOLS_SYSTEM_H
