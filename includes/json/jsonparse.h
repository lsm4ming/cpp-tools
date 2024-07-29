#pragma once

#include "common/common.h"
#include "json.h"

using namespace cpptools::common;

namespace cpptools::json
{
    class JsonDecode
    {
    private:
        String _str;

        static String parseString(std::istringstream *stream);

        static double parseNumber(std::istringstream *stream);

        JsonValuer parseObject(std::istringstream *stream);

        JsonValuer parseArray(std::istringstream *stream);

    public:
        explicit JsonDecode(String str) : _str(std::move(str))
        {};

        JsonValuer parseJsonValue();
    };

    class JsonParse
    {
    public:
        static JsonValue parse(const String &str);
    };
}
