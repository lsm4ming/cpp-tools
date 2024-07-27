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

        String parseString();

        double parseNumber();

        JsonObject parseObject();

        JsonObject parseObject(const String &str);

        JsonArray parseArray();

    public:
        explicit JsonDecode(String str) : _str(std::move(str))
        {};

        String parseString(std::istringstream istringstream);

        JsonValue parseJsonValue();
    };

    class JsonParse
    {
    public:
        static JsonValue parse(const String &str);
    };
}
