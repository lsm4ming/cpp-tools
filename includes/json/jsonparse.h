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
        static JsonValuer parse(const String &str);
    };
}
