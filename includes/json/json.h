#pragma once

#include <stdexcept>
#include <type_traits>
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

    using JsonVariant = std::variant<std::nullptr_t, bool, int, double, String, JsonObject, JsonArray>;

    class JsonValue
    {
    private:
        JsonVariant value;
        JsonToken json_type;

        JsonValue(JsonToken type, JsonVariant value) : json_type(type), value(value) {};

    public:
        explicit JsonValue() : JsonValue(JsonToken::NullValue, nullptr) {};

        explicit JsonValue(bool val) : JsonValue(JsonToken::BoolValue, val) {};

        explicit JsonValue(int val) : JsonValue(JsonToken::IntValue, val) {};

        explicit JsonValue(double val) : JsonValue(JsonToken::DoubleValue, val) {};

        explicit JsonValue(const String &val) : JsonValue(JsonToken::StringValue, val) {};

        explicit JsonValue(const JsonObject &val) : JsonValue(JsonToken::ObjectValue, val) {};

        explicit JsonValue(const JsonArray &val) : JsonValue(JsonToken::ArrayValue, val) {};

        JsonValue(const JsonValue &other) : json_type(other.json_type), value(other.value) {};

        JsonValue(JsonValue &&other) noexcept : json_type(other.json_type), value(std::move(other.value)) {};

        JsonValue &operator=(const JsonValue &other);

        JsonValue &operator=(JsonValue &&other) noexcept;

        ~JsonValue();

    public:
        // std::ostream &operator<<(std::ostream &os, const JsonValue &json_value)

        static JsonValue nullValue();

        static JsonValue objectValue();

        JsonValue operator[](int index) const;

        JsonValue operator[](const String &key) const;

        JsonValue &operator[](const String &key);

        JsonValue &operator=(const String &str);

        [[nodiscard]] JsonToken getType() const;
    };

    class JsonObject : public JsonValue
    {
    private:
        SortMap<String, JsonValue> value{};

    public:
        JsonObject() = default;

    public:
        JsonValue operator[](const String &key) const;

        void insert(const String &key, const JsonValue &val);

        std::pair<String, JsonValue> emplace(const String &key, const JsonValue &val);
    };

    class JsonArray : public JsonValue
    {
    private:
        List<JsonValue> value{};

    public:
        JsonArray() = default;

    public:
        JsonValue operator[](int index) const;

        [[nodiscard]] int size() const;

        void push_back(const JsonValue &val);
    };
}