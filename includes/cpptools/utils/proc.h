#pragma once

#include <cstring>
#include "cpptools/common/common.h"
#if defined(OS_MAC)
#include <print>
#endif

using namespace cpptools::common;

namespace cpptools::utils
{
    class Proc
    {
    public:
        /**
         * 获取当前进程名
         * @return
         */
        static String getSelfNam();
    };
}