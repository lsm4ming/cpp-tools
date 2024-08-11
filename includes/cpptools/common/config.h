#pragma once

#include <cstring>
#include <utility>
#include <variant>
#include <fstream>
#include <sys/stat.h>
#include "cpptools/utils/strings.h"
#include "cpptools/common/common.h"

using namespace cpptools::utils;

namespace cpptools::common
{
    class IniConfig
    {
    private:
        SortMap<String, SortMap<String, String>> _cfg;
        String _path;

    public:
        explicit IniConfig(String path="") : _path(std::move(path)) {};

        bool load(const String &path="");

        [[nodiscard]] size_t save(const String &path) const;

        [[nodiscard]] String getStrValue(const String &section, const String &key, const String &defaultValue) const;

        [[nodiscard]] int64 getIntValue(const String &section, const String &key, int64 defaultValue) const;

    private:
        static bool parseLine(String &line, String &section, String &key, String &value);

        void setSectionKeyValue(String &section, String &key, String &value);
    };
}