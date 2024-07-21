#pragma once

#include <chrono>
#include "time_zone.h"

using namespace cpptools::common;

namespace cpptools::time
{
#ifdef OS_LINUX
#undef unix // 取消宏定义
#endif
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
                         int minute = 0, int second = 0, TimeZone zone = getLocalTimeZone());

        void addDate(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

        [[nodiscard]] int64 unix() const;

        [[nodiscard]] int64 unixMillisecond() const;

        [[nodiscard]] int64 unixNanosecond() const;

        [[nodiscard]] String format(const String &format, TimeZone zone = getLocalTimeZone()) const;

        [[nodiscard]] bool before(const Time &time) const;

        [[nodiscard]] bool after(const Time &time) const;

        [[nodiscard]] int weekOfYear() const;

        [[nodiscard]] String dayOfWeek() const;

        Time add(Duration duration) const;
    };
}
