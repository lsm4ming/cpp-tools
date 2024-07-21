#include "time/time_zone.h"

namespace cpptools::time
{
    TimeZone defaultTimeZone = UTC;

    void setTimeZone(TimeZone timeZone)
    {
        defaultTimeZone = timeZone;
    }

    TimeZone getLocalTimeZone()
    {
        std::time_t now = std::time(nullptr);
        std::tm *local_time = std::localtime(&now);

        if (local_time != nullptr)
        {
            long offset = local_time->tm_gmtoff / 3600; // 偏移量（小时）
        }
        return defaultTimeZone;
    }
}