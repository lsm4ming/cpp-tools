#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace cpptools::common
{
#ifdef defined(WIN32)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#elif defined(__APPLE__)
#define OS_MAC
#endif

    using uint32 = unsigned int;
    using uint64 = unsigned long;
    using int64 = long long;

    using String = std::string;

    using StringList = std::vector<String>;

    template <typename T>
    using Vector = std::vector<T>;

    template <typename T>
    using VectorList = std::vector<Vector<T>>;

    using StringMap = std::unordered_map<String, String>;

    using StringMapList = std::vector<StringMap>;

    using TimePoint = std::chrono::system_clock::time_point;
}
