#pragma once

#include <cstring>
#include <variant>
#include <fstream>
#include <sys/stat.h>
#include "strings.h"
#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::utils
{
    class IniConfig
    {
    private:
        SortMap<String, SortMap<String, String>> _cfg;

    public:
        bool load(const String &path);

        size_t save(const String &path);

        String getStrValue(const String &section, const String &key, const String &defaultValue);

        int64 getIntValue(const String &section, const String &key, int64 defaultValue);

    private:
        static bool parseLine(String &line, String &section, String &key, String &value);

        void setSectionKeyValue(String &section, String &key, String &value);
    };
}