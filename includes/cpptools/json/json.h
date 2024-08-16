#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <sstream>
#include "cpptools/common/common.h"

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

    using JsonValuer = SharedPtr<JsonValue>;

    using JsonVariant = std::variant<std::nullptr_t, bool, int, double, String,
            SharedPtr < JsonObject>, SharedPtr <JsonArray>>;

    class JsonValue
    {
    private:
        JsonVariant value;
        JsonToken json_type;

        explicit JsonValue(JsonToken type, JsonVariant value) : json_type(type), value(std::move(value))
        {};

        void checkType(JsonToken target);

    public:
        explicit JsonValue() : JsonValue(JsonToken::NullValue, nullptr)
        {};

        explicit JsonValue(bool val) : JsonValue(JsonToken::BoolValue, val)
        {
        };

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

        explicit JsonValue(const StringView &val) : JsonValue(JsonToken::StringValue, String(val))
        {}

        JsonValue(const JsonValue &other) = default;

        JsonValue(JsonValue &&other) noexcept: json_type(other.json_type), value(std::move(other.value))
        {};

        JsonValue &operator=(const JsonValue &other);

        JsonValue &operator=(JsonValue &&other) noexcept;

        ~JsonValue();

    public:
        friend std::ostream &operator<<(std::ostream &os, const JsonValue &val);

        JsonValue &operator[](int index);

        JsonValue &operator[](const String &key);

        [[nodiscard]] JsonToken getType() const;

        // 处理字符串字面量的重载
        template<std::size_t N>
        JsonValue &operator=(const char (&arr)[N])
        {
            json_type = JsonToken::StringValue;
            value = arr;
            return *this;
        }

        JsonValue &operator=(const StringView &val)
        {
            json_type = JsonToken::StringValue;
            value = String(val);
            return *this;
        }

        template<class T>
        JsonValue &operator=(const T &val)
        {
            static_assert(!std::is_pointer<T>::value || std::is_same<T, const char *>::value,
                          "不支持将指针类型分配给JsonValue");
            if constexpr (std::is_same<T, bool>::value)
            {
                json_type = JsonToken::BoolValue;
            } else if constexpr (std::is_same<T, int>::value)
            {
                json_type = JsonToken::IntValue;
            } else if constexpr (std::is_same<T, double>::value)
            {
                json_type = JsonToken::DoubleValue;
            } else if constexpr (std::is_same<T, char *>::value)
            {
                json_type = JsonToken::StringValue;
            } else if constexpr (std::is_same<T, char (&)[]>::value)
            {
                json_type = JsonToken::StringValue;
            } else if constexpr (std::is_same<T, String>::value)
            {
                json_type = JsonToken::StringValue;
            } else if constexpr (std::is_same<T, JsonObject>::value || std::is_same<T, SharedPtr<JsonObject>>::value)
            {
                json_type = JsonToken::ObjectValue;
            } else if constexpr (std::is_same<T, JsonArray>::value || std::is_same<T, SharedPtr<JsonArray>>::value)
            {
                json_type = JsonToken::ArrayValue;
            } else
            {
                json_type = JsonToken::NullValue;
            }
            value = static_cast<const JsonVariant>(val);
            return *this;
        }

        [[nodiscard]] virtual String toString() const;
    };

    class JsonObject : public JsonValue
    {
    private:
        SortMap <String, JsonValuer> value{};

    public:
        JsonObject() = default;

        static SharedPtr <JsonObject> newObject();

    public:
        void insert(const String &key, const JsonValuer &val);

        JsonValuer get(const String &key);

        JsonValuer &emplace(const String &key, const JsonValuer &val);

        String toString() const override;
    };

    class JsonArray : public JsonValue
    {
    private:
        List <JsonValuer> value{};

    public:
        JsonArray() = default;

        static SharedPtr <JsonArray> newArray();

    public:
        [[nodiscard]] int size() const;

        void push_back(const JsonValuer &val);

        template<std::size_t N>
        void push_back(const char (&arr)[N])
        {
            this->value.push_back(std::make_shared<JsonValue>(String(arr)));
        }

        // 模板成员函数
        template<typename T>
        void push_back(const T &val)
        {
            this->value.push_back(std::make_shared<JsonValue>(val));
        }

        JsonValuer &get(int index);

        String toString() const override;
    };
}