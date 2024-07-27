#include "json/json.h"

#include <memory>

using namespace cpptools::json;

JsonValue JsonValue::operator[](int index)
{
    if (this->json_type != JsonToken::ArrayValue)
    {
        return JsonValue();
    }
    const auto &array = std::get<SharedPtr<JsonArray>>(value);
    return array.get()[index];
}

JsonValue JsonValue::nullValue()
{
    return JsonValue(JsonToken::NullValue, nullptr);
}

JsonValue JsonValue::objectValue()
{
    return JsonValue(JsonToken::ObjectValue, nullptr);
}

//JsonValue &JsonValue::operator[](const String &key)
//{
//    if (this->json_type != JsonToken::ObjectValue)
//    {
//        throw std::runtime_error("json_type is not object");
//    } else if (!std::holds_alternative<SharedPtr<JsonObject>>(value))
//    {
//        value = std::make_shared<JsonObject>();
//    }
//    const auto obj = std::get<SharedPtr<JsonObject>>(value);
//    auto result = obj->emplace(key, JsonValue());
//    return obj->operator[](key);
//}

JsonValue &JsonValue::operator[](const String &key)
{
    if (json_type != JsonToken::ObjectValue)
    {
        throw std::runtime_error("json_type is not object");
    }
    if (!std::holds_alternative<SharedPtr<JsonObject>>(value))
    {
        value = std::make_shared<JsonObject>();
    }
    auto obj = std::get<SharedPtr<JsonObject>>(value);
    return obj->emplace(key, JsonValue());
}

JsonValue &JsonValue::operator=(const String &str)
{
    this->json_type = JsonToken::StringValue;
    this->value = str;
    return *this;
}

JsonValue &JsonValue::operator=(const JsonValue &other)
{
    if (this != &other)
    {
        json_type = other.json_type;
        value = other.value;
    }
    return *this;
}

JsonValue &JsonValue::operator=(JsonValue &&other) noexcept
{
    if (this != &other)
    {
        json_type = other.json_type;
        value = std::move(other.value);
    }
    return *this;
}

JsonToken JsonValue::getType() const
{
    return this->json_type;
}

JsonValue::~JsonValue() = default;

void JsonObject::insert(const String &key, const JsonValue &val)
{
    this->value[key] = val;
}

JsonValue JsonObject::get(const String &key)
{
    return this->value[key];
}

JsonValue &JsonObject::emplace(const String &key, const JsonValue &val)
{
    auto [iter, ok] = this->value.emplace(key, val);
    return iter->second;
}

int JsonArray::size() const
{
    return static_cast<int>(value.size());
}

void JsonArray::push_back(const JsonValue &val)
{
    this->value.push_back(val);
}