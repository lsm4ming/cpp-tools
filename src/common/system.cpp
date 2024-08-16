#include "cpptools/common/system.h"

namespace cpptools::common
{
    uint64 formatWithMemoryUnit(uint64 bytes, const String &unit)
    {
        if (unit == "KB")
        {
            return bytes / 1024;
        } else if (unit == "MB")
        {
            return bytes / 1024 / 1024;
        } else if (unit == "GB")
        {
            return bytes / 1024 / 1024 / 1024;
        } else if (unit == "TB")
        {
            return bytes / 1024 / 1024 / 1024 / 1024;
        } else
        {
            return bytes;
        }
    }

    void System::exit(int code)
    {
        std::exit(code);
    }

    void System::sleep(uint32 ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    uint32 System::cpuNumber()
    {
        return std::thread::hardware_concurrency();
    }

    uint64 System::getTotalMemory(const String &unit)
    {
        uint64 totalMemory{};
        struct sysinfo info{};
        if (sysinfo(&info) == 0)
        {
            totalMemory = info.totalram;
            totalMemory *= info.mem_unit;
        }
        return formatWithMemoryUnit(totalMemory, unit);
    }

    uint64 System::getAvailableMemory(const String &unit)
    {
        uint64 totalMemory{};
        struct sysinfo info{};
        if (sysinfo(&info) == 0)
        {
            totalMemory = info.freeram;
            totalMemory *= info.mem_unit;
        }
        return formatWithMemoryUnit(totalMemory, unit);
    }

    uint64 System::getUsedMemory(const String &unit)
    {
        return getTotalMemory(unit) - getAvailableMemory(unit);
    }
}