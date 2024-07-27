#include "json/json.h"

using namespace cpptools::json;

JsonValue JsonValue::operator[](int index) const
{
    if (this->json_type != JsonToken::ArrayValue)
    {
        return JsonValue();
    }
    auto &array = std::get<JsonArray>(value);
    return array[index];
}

JsonValue JsonValue::nullValue()
{
    return JsonValue(JsonToken::NullValue, nullptr);
}

JsonValue JsonValue::objectValue()
{
    return JsonValue(JsonToken::ObjectValue, nullptr);
}

JsonValue JsonValue::operator[](const String &key) const
{
    if (this->json_type != JsonToken::ObjectValue)
    {
        return JsonValue();
    }
    const auto &obj = std::get<JsonObject>(value);
    return obj[key];
}

JsonValue &JsonValue::operator[](const String &key)
{
    if (json_type != JsonToken::ObjectValue)
    {
        throw std::runtime_error("JsonValue is not an object.");
    }
    auto &obj = std::get<JsonObject>(value);
    // 如果键不存在，则插入一个默认构造的 JsonValue
    auto [iter, inserted] = obj.emplace(key, JsonValue());
    return iter; // 返回对应键的引用
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

JsonValue::~JsonValue()
{
}

JsonValue JsonObject::operator[](const String &key) const
{
    return this->value.at(key);
}

void JsonObject::insert(const String &key, const JsonValue &val)
{
    this->value[key] = val;
}

std::pair<String, JsonValue> JsonObject::emplace(const String &key, const JsonValue &val)
{
    return this->value.emplace(key, val);
}

JsonValue JsonArray::operator[](int index) const
{
    return this->value[index];
}

int JsonArray::size() const
{
    return this->value.size();
}

void JsonArray::push_back(const JsonValue &val)
{
    this->value.push_back(val);
}