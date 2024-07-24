#pragma once

#include "common/common.h"
#include "json.h"

using namespace cpptools::common;

namespace cpptools::json
{
    class JsonParse
    {
    public:
        static JsonValue parse(const String &str);
    };
}
