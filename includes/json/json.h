#pragma once

#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::json
{
    class JsonValue
    {
    };

    class JsonObject : public JsonValue
    {
    private:
        SortMap <String, JsonValue> value;
    };

    class JsonArray : public JsonValue
    {
    private:
        VectorList <JsonValue> value;
    };

    class JsonString : public JsonValue
    {
    private:
        String value;
    };

    class JsonBool : public JsonValue
    {
    private:
        bool value;
    };

    class JsonInt : public JsonValue
    {
    private:
        int value;
    };

    class JsonDouble : public JsonValue
    {
    private:
        double value;
    };
}