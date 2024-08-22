#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <sstream>
#include <thread>
#include <functional>
#include <unordered_set>
#include <tuple>

namespace cpptools::common
{
#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC
#endif

    using uint32 = unsigned int;
    using uint64 = unsigned long;
    using int64 = long long;
    using uint16 = unsigned short;

    using String = std::string;

    using StringView = std::string_view;

    using StringList = std::vector<String>;

    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using VectorList = std::vector<Vector<T>>;

    template<typename T>
    using List = std::vector<T>;

    using StringMap = std::unordered_map<String, String>;

    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    template<typename K, typename V>
    using SortMap = std::map<K, V>;

    using StringMapList = std::vector<StringMap>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    using OsStringStream = std::ostringstream;

    using Thread = std::thread;

    template<typename T>
    using Function = std::function<T>;

    template<typename T>
    using HashSet = std::unordered_set<T>;

    template<typename ...T>
    using Tuple = std::tuple<T...>;
}
