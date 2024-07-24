#pragma once

#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::json
{
    enum JsonToken
    {
        NullValue,
        BoolValue,
        StringValue,
        IntValue,
        DoubleValue,
        ArrayValue,
        ObjectValue
    };

    class JsonValue;

    class JsonObject
    {
    private:
        SortMap<String, JsonValue> value{};

    public:
        JsonObject() = default;

    public:
        JsonValue operator[](const String &key) const;
    };

    class JsonArray
    {
    private:
        VectorList<JsonValue> value{};

    public:
        JsonArray() = default;

    public:
        JsonValue operator[](int index) const;
    };

    class JsonValue
    {
    private:
        union json_value
        {
            int int_value;
            double double_value;
            bool bool_value;
            String string_value;
            JsonObject object_value;
            JsonArray array_value;
        };

        JsonToken json_type;

    public:
        JsonValue() = default;

    public:
        virtual ~JsonValue() = default;

        JsonValue operator[](int index) const;

        JsonValue operator[](const String &key) const;

        JsonToken getType() const;
    };
}