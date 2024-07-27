#pragma once

#include <sstream>
#include <utility>
#include "common/common.h"
#include "json.h"

using namespace cpptools::common;

namespace cpptools::json
{
    class JsonDecode
    {
    private:
        String _str;

        String parseString(std::istringstream *stream);

        double parseNumber(std::istringstream *stream);

        JsonObject parseObject(std::istringstream *stream);

        JsonArray parseArray(std::istringstream *stream);

    public:
        explicit JsonDecode(String str) : _str(std::move(str))
        {};

        JsonValue parseJsonValue();
    };

    class JsonParse
    {
    public:
        static JsonValue parse(const String &str);
    };
}
