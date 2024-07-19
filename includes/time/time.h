#pragma once

#include <chrono>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::time
{
#ifdef OS_LINUX
#undef unix // 取消宏定义
#endif

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

    class Time
    {
    private:
        TimePoint _time;

    public:
        Time() = default;

        explicit Time(TimePoint time) : _time(time){};

        ~Time() = default;

        static Time now();

        static Time date(int year, int month, int day, int hour = 0,
                         int minute = 0, int second = 0, TimeZone zone = defaultTimeZone);

        void addDate(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

        [[nodiscard]] int64 unix() const;

        [[nodiscard]] int64 unixMillisecond() const;

        [[nodiscard]] int64 unixNanosecond() const;

        [[nodiscard]] String format(const String &format, TimeZone zone = defaultTimeZone) const;

        [[nodiscard]] bool before(const Time &time) const;

        [[nodiscard]] bool after(const Time &time) const;

        [[nodiscard]] int weekOfYear() const;

        [[nodiscard]] String dayOfWeek() const;
    };
}
