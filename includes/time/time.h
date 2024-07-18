#pragma once

#include <chrono>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::time
{
    class Time
    {
    private:
        time_t _time = 0;
    public:
        Time() = default;
        ~Time() = default;

        static Time now();
    };
}
