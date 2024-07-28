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

std::ostream &cpptools::json::operator<<(std::ostream &os, const JsonValue &val)
{
    os << val.toString();
    return os;
}

String JsonValue::toString() const
{
    String result;
    switch (json_type)
    {
        case JsonToken::BoolValue:
            return std::get<bool>(value) ? "true" : "false";
        case JsonToken::IntValue:
            return std::to_string(std::get<int>(value));
        case JsonToken::DoubleValue:
            return std::to_string(std::get<double>(value));
        case JsonToken::StringValue:
            return "\"" + std::get<String>(value) + "\"";
        case JsonToken::ArrayValue:
            if (!std::holds_alternative<SharedPtr<JsonArray>>(value))
            {
                return "[]";
            }
            return std::get<SharedPtr<JsonArray>>(value)->toString();
        case JsonToken::ObjectValue:
            if (!std::holds_alternative<SharedPtr<JsonObject>>(value))
            {
                return "{}";
            }
            return std::get<SharedPtr<JsonObject>>(value)->toString();
        default:
            return "null";
    }
}

JsonValue::~JsonValue() =
default;

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

String JsonObject::toString() const
{
    std::ostringstream os;
    os << "{";
    for (auto it = value.begin(); it != value.end(); ++it)
    {
        os << "\"" << it->first << "\":" << it->second->toString();
        if (std::next(it) != value.end())
        { // 判断是否为最后一个元素
            os << ",";
        }
    }
    os << "}";
    return os.str();
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

String JsonArray::toString() const
{
    std::ostringstream os;
    os << "[";
    for (auto &item: this->value)
    {
        os << item->toString();
        if (item != this->value.back())
        {
            os << ",";
        }
    }
    os << "]";
    return os.str();
}
