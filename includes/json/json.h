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

    class JsonObject : public JsonValue
    {
    private:
        SortMap <String, JsonValue> value{};

    public:
        JsonObject() = default;

    public:
        JsonValue operator[](const String &key) const;
    };

    class JsonArray : public JsonValue
    {
    private:
        List <JsonValue> value{};

    public:
        JsonArray() = default;

    public:
        JsonValue operator[](int index) const;

        [[nodiscard]] int size() const;

        void push_back(const JsonValue &val);
    };

    union JsonData
    {
        int int_value;
        double double_value;
        bool bool_value;
        String *string_value;
        JsonObject *object_value;
        JsonArray *array_value;
    };

    class JsonValue
    {
    private:
        JsonData json_data{};
        JsonToken json_type;

        JsonValue(JsonToken type, JsonData data) : json_type(type), json_data(data)
        {};

    public:
        explicit JsonValue() = default;

        explicit JsonValue(bool val) : JsonValue(JsonToken::BoolValue, JsonData{.bool_value =  val})
        {};

        explicit JsonValue(int val) : JsonValue(JsonToken::IntValue, JsonData{.int_value =  val})
        {};

        explicit JsonValue(double val) : JsonValue(JsonToken::DoubleValue, JsonData{.double_value =  val})
        {};

        explicit JsonValue(const String &val): JsonValue(JsonToken::StringValue, JsonData{.string_value =  new String(val)})
        {};

        ~JsonValue();

    public:
        JsonValue operator[](int index) const;

        JsonValue operator[](const String &key) const;

        [[nodiscard]] JsonToken getType() const;
    };
}