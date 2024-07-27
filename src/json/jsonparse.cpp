#include "json/jsonparse.h"

using namespace cpptools::json;

JsonValue JsonParse::parse(const String &str)
{
    JsonDecode decode(str);
    return decode.parseJsonValue();
}

double JsonDecode::parseNumber()
{
    std::string numberStr;
    char ch;
    while (stream.get(ch))
    {
        if (!std::isdigit(ch) && ch != '.' && ch != '-')
        {
            stream.putback(ch);
            break;
        }
        numberStr += ch;
    }
    return std::stod(numberStr);
}

String JsonDecode::parseString()
{
    String result;
    char ch;
    while (stream.get(ch))
    {
        if (ch == '"')
        {
            break;
        }
        result += ch;
    }
    return result;
}

JsonObject JsonDecode::parseObject(const String &str)
{
    JsonObject jsonObject;
    std::istringstream stream(str);
    char ch;
    stream >> ch; // 读取起始大括号
    if (ch != '{')
    {
        throw std::runtime_error("Invalid JSON format: missing '{'");
    }
    while (stream >> ch)
    {
        if (ch == '}')
        {
            break;
        }
        if (ch == ',')
        {
            continue;
        }
        if (ch == '"')
        {
            std::string key = parseString();
            stream >> ch; // 读取冒号
            if (ch != ':')
            {
                throw std::runtime_error("Invalid JSON format: missing ':'");
            }
            stream >> ch; // 读取值起始
            if (ch == '"')
            {
                jsonObject[key] = parseString(stream);
            } else if (std::isdigit(ch) || ch == '-')
            {
                stream.putback(ch);
                jsonObject[key] = parseNumber();
            } else if (ch == '{')
            {
                std::string subObject = "{";
                int braceCount = 1;
                while (braceCount > 0 && stream.get(ch))
                {
                    subObject += ch;
                    if (ch == '{') braceCount++;
                    if (ch == '}') braceCount--;
                }
                jsonObject[key] = parseObject(subObject);
            } else if (ch == '[')
            {
                std::string subArray = "[";
                int bracketCount = 1;
                while (bracketCount > 0 && stream.get(ch))
                {
                    subArray += ch;
                    if (ch == '[') bracketCount++;
                    if (ch == ']') bracketCount--;
                }
                jsonObject[key] = parseArray(subArray);
            }
        } else
        {
            throw std::runtime_error("Invalid JSON format: missing '\"'");
        }
    }
    return jsonObject;
}

JsonObject JsonDecode::parseObject()
{
    return this->parseObject(this->_str);
}

String JsonDecode::parseString(std::istringstream istringstream)
{
    return {};
}

JsonValue JsonDecode::parseJsonValue()
{
    return JsonValue();
}
