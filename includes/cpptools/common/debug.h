#pragma once

#include <sys/resource.h>
#include "common.h"
#if defined(OS_MAC)
#include <sys/sysctl.h>
#elif defined(OS_LINUX)
#include <sys/sysinfo.h>
#endif

namespace cpptools::common
{
    class Debug
    {
    private:
        bool _debug;

        Debug() = default;

    public:
        /**
         * 设置是否开启debug
         * @param debug
         */
        static void setDebug(bool debug);

        /**
         * 打开coredump
         */
        static void coreDumpEnable();

        /**
         * 是否开启debug
         * @return
         */
        static bool isDebug();

        static Debug& getInstance()
        {
            static Debug instance;
            return instance;
        }
    };
}