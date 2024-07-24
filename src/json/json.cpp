#include "json/json.h"

using namespace cpptools::json;

JsonValue JsonValue::operator[](int index) const
{
    if (this->json_type != JsonToken::ArrayValue)
    {
        return JsonValue();
    }
    JsonArray array = static_cast<JsonArray>(this->json_value);
    return array[index];
}

JsonValue JsonValue::operator[](const String &key) const
{
    if (this->json_type != JsonToken::ObjectValue)
    {
        return JsonValue();
    }
    return values[key];
}

JsonToken JsonValue::getType() const
{
    return this->json_type;
}

JsonValue JsonObject::operator[](const String & key) const
{
    return this->value[key];
}

JsonValue JsonArray::operator[](int index) const
{
    return this->value[index];
}
