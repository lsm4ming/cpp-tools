#include "time/time.h"

namespace cpptools::time
{
    Time Time::now()
    {
        Time time;
        time._time = std::chrono::system_clock::now();
        return time;
    }
}
