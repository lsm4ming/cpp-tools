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
#if defined(OS_LINUX)
        struct sysinfo info{};
        if (sysinfo(&info) == 0)
        {
            totalMemory = info.totalram;
            totalMemory *= info.mem_unit;
        }
#elif defined(OS_MAC)
        mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
        host_basic_info_data_t host_info;
        kern_return_t kr = host_statistics(mach_host_self(), HOST_BASIC_INFO, (host_info_t) &host_info, &count);
        if (kr == KERN_SUCCESS)
        {
            totalMemory = static_cast<uint64_t>(host_info.max_mem);
        }
#endif
        return formatWithMemoryUnit(totalMemory, unit);
    }

    uint64 System::getAvailableMemory(const String &unit)
    {
        uint64 totalMemory{};
#if defined(OS_LINUX)
        struct sysinfo info{};
        if (sysinfo(&info) == 0)
        {
            totalMemory = info.freeram;
            totalMemory *= info.mem_unit;
        }
#elif defined(OS_MAC)
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        vm_statistics64_data_t vm_info;
        kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO64, (host_info_t) &vm_info, &count);
        if (kr == KERN_SUCCESS)
        {
            totalMemory = static_cast<uint64_t>(vm_info.free_count) * sysconf(_SC_PAGESIZE);
        }
#endif
        return formatWithMemoryUnit(totalMemory, unit);
    }

    uint64 System::getUsedMemory(const String &unit)
    {
        return getTotalMemory(unit) - getAvailableMemory(unit);
    }
}