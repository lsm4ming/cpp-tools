#include "json/json.h"

using namespace cpptools::json;

JsonValue JsonValue::operator[](int index) const
{
    if (this->json_type != JsonToken::ArrayValue)
    {
        return JsonValue();
    }
    return this->json_data.array_value->operator[](index);
}

JsonValue JsonValue::operator[](const String &key) const
{
    if (this->json_type != JsonToken::ObjectValue)
    {
        return JsonValue();
    }
    return this->json_data.object_value->operator[](key);
}

JsonToken JsonValue::getType() const
{
    return this->json_type;
}

JsonValue::~JsonValue()
{
    if (this->json_type == JsonToken::ArrayValue)
    {
        delete this->json_data.array_value;
        this->json_data.array_value = nullptr;
    } else if (this->json_type == JsonToken::ObjectValue)
    {
        delete this->json_data.object_value;
        this->json_data.object_value = nullptr;
    } else if (this->json_type == JsonToken::StringValue)
    {
        delete this->json_data.string_value;
        this->json_data.string_value = nullptr;
    }
}

JsonValue JsonObject::operator[](const String &key) const
{
    return this->value.at(key);
}

JsonValue JsonArray::operator[](int index) const
{
    return this->value[index];
}
