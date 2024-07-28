#include "json/json.h"

using namespace cpptools::json;

JsonValue &JsonValue::operator[](int index)
{
    this->checkType(JsonToken::ArrayValue);
    return *(std::get<SharedPtr<JsonArray>>(value)->get(index));
}

JsonValue &JsonValue::operator[](const String &key)
{
    this->checkType(JsonToken::ObjectValue);
    if (!std::holds_alternative<SharedPtr<JsonObject>>(value))
    {
        value = std::make_shared<JsonObject>();
    }
    auto object = std::get<SharedPtr<JsonObject>>(value);
    return *object->emplace(key, std::make_shared<JsonValue>(JsonValue{}));
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

void JsonValue::checkType(JsonToken target)
{
    if (json_type == JsonToken::NullValue)
    {
        json_type = target;
    } else if (json_type != target)
    {
        throw std::runtime_error("Invalid JSON format: invalid type");
    }
}

JsonValue::~JsonValue() = default;

void JsonObject::insert(const String &key, const JsonValuer &val)
{
    this->value[key] = val;
}

JsonValuer JsonObject::get(const String &key)
{
    return this->value[key];
}

JsonValuer &JsonObject::emplace(const String &key, const JsonValuer &val)
{
    auto [iter, ok] = this->value.emplace(key, val);
    return iter->second;
}

int JsonArray::size() const
{
    return static_cast<int>(value.size());
}

void JsonArray::push_back(const JsonValuer &val)
{
    this->value.push_back(val);
}

JsonValuer &JsonArray::get(int index)
{
    return this->value.at(index);
}
