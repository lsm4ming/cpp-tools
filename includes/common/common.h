#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace cpptools::common
{
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
}
