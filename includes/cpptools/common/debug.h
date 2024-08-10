#pragma once

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include "common.h"

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