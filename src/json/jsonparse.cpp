#include "json/jsonparse.h"

using namespace cpptools::json;

JsonValue JsonParse::parse(const String &str)
{
    JsonDecode decode(str);
    auto result =decode.parseJsonValue();
    // 判断智能指针是否为空
    if (result == nullptr)
    {
        throw std::runtime_error("Invalid JSON format: null pointer");
    }
    return *result;
}

double JsonDecode::parseNumber(std::istringstream *stream)
{
    std::string numberStr;
    char ch;
    while (stream->get(ch))
    {
        if (!std::isdigit(ch) && ch != '.' && ch != '-')
        {
            stream->putback(ch);
            break;
        }
        numberStr += ch;
    }
    // 是不是浮点数
    // if (numberStr.find('.') != std::string::npos)
    // {
    // }
    return std::stod(numberStr);
}

String JsonDecode::parseString(std::istringstream *stream)
{
    String result;
    char ch;
    while (stream->get(ch))
    {
        if (ch == '"')
        {
            break;
        }
        result += ch;
    }
    return result;
}

JsonValuer JsonDecode::parseObject(std::istringstream *stream)
{
    auto jsonObject = std::make_shared<JsonObject>();
    char ch;
    *stream >> ch; // 读取起始大括号
    if (ch != '{')
    {
        throw std::runtime_error("Invalid JSON format: missing '{'");
    }
    while (*stream >> ch)
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
            std::string key = parseString(stream);
            *stream >> ch; // 读取冒号
            if (ch != ':')
            {
                throw std::runtime_error("Invalid JSON format: missing ':'");
            }
            *stream >> ch; // 读取值起始
            if (ch == '"')
            {
                jsonObject->insert(key, std::make_shared<JsonValue>(parseString(stream)));
            } else if (std::isdigit(ch) || ch == '-')
            {
                stream->putback(ch);
                jsonObject->insert(key, std::make_shared<JsonValue>(parseNumber(stream)));
            } else if (ch == '{')
            {
                std::string subObject = "{";
                int braceCount = 1;
                while (braceCount > 0 && stream->get(ch))
                {
                    subObject += ch;
                    if (ch == '{')
                        braceCount++;
                    if (ch == '}')
                        braceCount--;
                }
                std::istringstream subStream(subObject);
                jsonObject->insert(key, parseObject(&subStream));
            } else if (ch == '[')
            {
                std::string subArray = "[";
                int bracketCount = 1;
                while (bracketCount > 0 && stream->get(ch))
                {
                    subArray += ch;
                    if (ch == '[')
                        bracketCount++;
                    if (ch == ']')
                        bracketCount--;
                }
                std::istringstream subStream(subArray);
                jsonObject->insert(key, parseArray(&subStream));
            }
        } else
        {
            throw std::runtime_error("Invalid JSON format: missing '\"'");
        }
    }
    return std::make_shared<JsonValue>(jsonObject);
}

JsonValuer JsonDecode::parseArray(std::istringstream *stream)
{
    auto jsonArray = std::make_shared<JsonArray>();
    char ch;
    *stream >> ch; // 读取起始括号
    if (ch != '[')
    {
        throw std::runtime_error("Invalid JSON format: missing '['");
    }
    int level = 0;
    while (*stream >> ch)
    {
        if (ch == '[')
        {
            level++;
        } else if (ch == ']')
        {
            level--;
        } else if (ch == ',')
        {
            continue;
        }
        if (ch == '"')
        {
            jsonArray->push_back(std::make_shared<JsonValue>(JsonValue(parseString(stream))));
        } else if (std::isdigit(ch) || ch == '-')
        {
            stream->putback(ch);
            jsonArray->push_back(std::make_shared<JsonValue>(JsonValue(parseNumber(stream))));
        } else if (ch == '{')
        {
            jsonArray->push_back(parseObject(stream));
        } else if (ch == '[')
        {
            jsonArray->push_back(parseArray(stream));
        }
    }
    return std::make_shared<JsonValue>(jsonArray);
}

JsonValuer JsonDecode::parseJsonValue()
{
    // 去除前面的空格和换行
    _str.erase(0, _str.find_first_not_of(" \t\r\n"));
    // 判断是JSON对象还是JSON数组
    std::istringstream stream(_str);
    if (_str[0] == '{')
    {
        return parseObject(&stream);
    } else if (_str[0] == '[')
    {
        return parseArray(&stream);
    }
    throw std::runtime_error("invalid character 'h' looking for beginning of value");
}
