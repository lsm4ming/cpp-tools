#include "time/time.h"

namespace cpptools::time
{
    TimeZone defaultTimeZone = UTC;

    void setTimeZone(TimeZone timeZone)
    {
        defaultTimeZone = timeZone;
    }

    Time Time::now()
    {
        return Time(std::chrono::system_clock::now());
    }

    Time Time::date(int year, int month, int day, int hour, int minute, int second, TimeZone zone)
    {
        std::tm timeInfo = {};
        timeInfo.tm_year = year - 1900;
        timeInfo.tm_mon = month - 1;
        timeInfo.tm_mday = day;
        timeInfo.tm_hour = hour - zone;
        timeInfo.tm_min = minute;
        timeInfo.tm_sec = second;
        auto t = std::mktime(&timeInfo);
        return Time(std::chrono::system_clock::from_time_t(t));
    }

    int64 Time::unix() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(_time.time_since_epoch()).count();
    }

    int64 Time::unixMillisecond() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(_time.time_since_epoch()).count();
    }

    int64 Time::unixNanosecond() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(_time.time_since_epoch()).count();
    }

    String Time::format(const String &format, TimeZone zone) const
    {
        auto adjustedTime = _time + std::chrono::hours(zone);
        std::time_t t = std::chrono::system_clock::to_time_t(adjustedTime);
        std::tm tm{};
        gmtime_r(&t, &tm);
        char buffer[256];
        std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);
        return buffer;
    }

    void Time::addDate(int year, int month, int day, int hour, int minute, int second)
    {
        this->_time += std::chrono::years(year);
        this->_time += std::chrono::months(month);
        this->_time += std::chrono::days(day);
        this->_time += std::chrono::hours(hour);
        this->_time += std::chrono::minutes(minute);
        this->_time += std::chrono::seconds(second);
    }

    bool Time::before(const Time &time) const
    {
        return this->_time < time._time;
    }

    bool Time::after(const Time &time) const
    {
        return this->_time > time._time;
    }

    int Time::week() const
    {
        std::time_t t = std::chrono::system_clock::to_time_t(this->_time);
        std::tm tm{};
        gmtime_r(&t, &tm);

        int dayOfYear = tm.tm_yday + 1;

        int weekNumber = (dayOfYear - (tm.tm_wday + 6) % 7 + 10) / 7;

        if (weekNumber == 0) // 处理第0周
        {
            std::tm prevYearTm = tm;
            prevYearTm.tm_year -= 1;
            std::time_t prevYearTimeT = std::mktime(&prevYearTm);
            std::tm *prevYearPtr = std::gmtime(&prevYearTimeT);
            int prevYearDayOfYear = prevYearPtr->tm_yday + 1;
            weekNumber = (prevYearDayOfYear - (prevYearPtr->tm_wday + 6) % 7 + 10) / 7;
        } else if (weekNumber == 53) // 处理最后一周
        {
            std::tm nextYearTm = tm;
            nextYearTm.tm_year += 1;
            std::time_t nextYearTimeT = std::mktime(&nextYearTm);
            std::tm *nextYearPtr = std::gmtime(&nextYearTimeT);
            int nextYearDayOfYear = nextYearPtr->tm_yday + 1;
            if ((nextYearDayOfYear - (nextYearPtr->tm_wday + 6) % 7 + 10) / 7 != 1)
            {
                weekNumber = 1;
            }
        }

        return weekNumber;
    }
}
