#pragma once

#include <chrono>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::time
{
    using Duration = int64;
    using TimePoint = std::chrono::system_clock::time_point;

    const Duration MILLISECOND = 1000;
    const Duration SECOND = 1000 * MILLISECOND;
    const Duration MINUTE = 60 * SECOND;
    const Duration HOUR = 60 * MINUTE;
    const Duration DAY = 24 * HOUR;

    enum TimeZone
    {
        UTCMinus12 = -12,
        UTCMinus11 = -11,
        UTCMinus10 = -10,
        UTCMinus9 = -9,
        UTCMinus8 = -8,
        UTCMinus7 = -7,
        UTCMinus6 = -6,
        UTCMinus5 = -5,
        UTCMinus4 = -4,
        UTCMinus3 = -3,
        UTCMinus2 = -2,
        UTCMinus1 = -1,
        UTC = 0,
        UTCPlus1 = 1,
        UTCPlus2 = 2,
        UTCPlus3 = 3,
        UTCPlus4 = 4,
        UTCPlus5 = 5,
        UTCPlus6 = 6,
        UTCPlus7 = 7,
        UTCPlus8 = 8,
        UTCPlus9 = 9,
        UTCPlus10 = 10,
        UTCPlus11 = 11,
    };

    extern TimeZone defaultTimeZone;

    extern void setTimeZone(TimeZone timeZone);

    extern TimeZone getLocalTimeZone();
}