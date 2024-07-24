#pragma once

#include "common/common.h"
#include "json.h"

using namespace cpptools::common;

namespace cpptools::json
{
    enum JsonToken
    {
        Null,
        Bool,
        String,
        Int,
        Double,
        Array,
        Object
    };

    class JsonParse
    {
    public:
        static JsonValue parse(const ::String& str);
    };
}

