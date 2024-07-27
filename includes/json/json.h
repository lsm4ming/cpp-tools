#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
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

    class JsonObject;

    class JsonArray;

    using JsonVariant = std::variant<std::nullptr_t, bool, int, double, String,
            SharedPtr < JsonObject>, SharedPtr <JsonArray>>;

    class JsonValue
    {
    private:
        JsonVariant value;
        JsonToken json_type;

        explicit JsonValue(JsonToken type, JsonVariant value) : json_type(type), value(std::move(value))
        {};

    public:
        explicit JsonValue() : JsonValue(JsonToken::NullValue, nullptr)
        {};

        explicit JsonValue(bool val) : JsonValue(JsonToken::BoolValue, val)
        {};

        explicit JsonValue(int val) : JsonValue(JsonToken::IntValue, val)
        {};

        explicit JsonValue(double val) : JsonValue(JsonToken::DoubleValue, val)
        {};

        explicit JsonValue(const String &val) : JsonValue(JsonToken::StringValue, val)
        {};

        explicit JsonValue(const SharedPtr <JsonObject> &val) : JsonValue(JsonToken::ObjectValue, val)
        {};

        explicit JsonValue(const SharedPtr <JsonArray> &val) : JsonValue(JsonToken::ArrayValue, val)
        {};

        JsonValue(const JsonValue &other) : json_type(other.json_type), value(other.value)
        {};

        JsonValue(JsonValue &&other) noexcept: json_type(other.json_type), value(std::move(other.value))
        {};

        JsonValue &operator=(const JsonValue &other);

        JsonValue &operator=(JsonValue &&other) noexcept;

        ~JsonValue();

    public:
        // std::ostream &operator<<(std::ostream &os, const JsonValue &json_value)

        static JsonValue nullValue();

        static JsonValue objectValue();

        JsonValue operator[](int index);

        JsonValue &operator[](const String &key);

        // JsonValue &operator[](const String &key);

        JsonValue &operator=(const String &str);

        [[nodiscard]] JsonToken getType() const;
    };

    class JsonObject : public JsonValue
    {
    private:
        SortMap <String, JsonValue> value{};

    public:
        JsonObject() = default;

    public:
        void insert(const String &key, const JsonValue &val);

        JsonValue get(const String &key);

        JsonValue& emplace(const String &key, const JsonValue &val);
    };

    class JsonArray : public JsonValue
    {
    private:
        List <JsonValue> value{};

    public:
        JsonArray() = default;

    public:
        [[nodiscard]] int size() const;

        void push_back(const JsonValue &val);
    };
}