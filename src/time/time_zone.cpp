#include "time/time_zone.h"

namespace cpptools::time
{
    TimeZone *defaultTimeZone = nullptr;

    void setTimeZone(TimeZone timeZone)
    {
        if (defaultTimeZone != nullptr)
        {
            delete defaultTimeZone;
            defaultTimeZone = nullptr;
        }
        defaultTimeZone = new TimeZone;
        *defaultTimeZone = timeZone;
    }

    TimeZone getLocalTimeZone()
    {
        if (defaultTimeZone != nullptr)
        {
            return *defaultTimeZone;
        }
        std::time_t now = std::time(nullptr);
        std::tm *local_time = std::localtime(&now);

        if (local_time != nullptr)
        {
            return static_cast<TimeZone>(local_time->tm_gmtoff / 3600);  // 偏移量（小时）
        }
        return TimeZone::UTC;
    }
}