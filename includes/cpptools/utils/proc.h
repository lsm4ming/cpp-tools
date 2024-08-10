#pragma once

#include <cstring>
#include "cpptools/common/common.h"

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