#include "cpptools/tools/generator/protobuf/parser.h"

namespace cpptools::tools::generator::protobuf
{
    Vector<Tuple<Token, String>> ProtobufParser::parse2Token(const String &path)
    {
        if (path.empty()) throw std::invalid_argument("path is empty");
        std::ifstream in;
        String line;
        in.open(path.c_str());
        if (!in.is_open()) throw std::invalid_argument("open file error");

        auto tokens = Vector<Tuple<Token, String>>();
        Token token = INIT;
        String value;
        while (getline(in, line))
        {
            parseLine(line, value, token, tokens);
        }
        return tokens;
    }

    void
    ProtobufParser::parseLine(const String &line, String &value, Token &token, Vector<Tuple<Token, String>> &tokens)
    {
        auto getOneToken = [&value, &tokens, &token](Token newStatus)
        {
            if (!value.empty())
            {
                tokens.emplace_back(
                    token,
                    value
                );
            }
            value = "";
            token = newStatus;
        };
        for (size_t i = 0; i < line.size(); i++)
        {
            char c = line[i];
            if (token == INIT)
            {
                if (isblank(c)) continue;
                if (isdigit(c))
                {  // 数字
                    value = c;
                    token = ORDER_NUM;
                } else if (c == '/')
                {  // 注释
                    if (i + 1 < line.size() && (line[i + 1] == '*' || line[i + 1] == '/'))
                    {
                        token = COMMENT_BEGIN;
                    } else
                    {
                        value = c;
                        token = IDENTIFIER;
                    }
                } else if (isSpecial(c))
                {  // 特殊字符
                    value = c;
                    token = SPECIAL_CHAR;
                } else
                {  // 执行到这里就是标识符
                    value = c;
                    token = IDENTIFIER;
                }
            } else if (token == IDENTIFIER)
            {
                if (isblank(c))
                {
                    getOneToken(INIT);
                } else if (isdigit(c))
                {
                    value += c;
                } else if (c == '/')
                {
                    if (i + 1 < line.size() && (line[i + 1] == '*' || line[i + 1] == '/'))
                    {
                        getOneToken(COMMENT_BEGIN);
                    } else
                    {
                        value += c;
                    }
                } else if (isSpecial(c))
                {
                    getOneToken(SPECIAL_CHAR);
                    value = c;
                } else
                {
                    value += c;
                }
            } else if (token == SPECIAL_CHAR)
            {
                if (isblank(c))
                {
                    getOneToken(INIT);
                } else if (isdigit(c))
                {
                    getOneToken(ORDER_NUM);
                    value = c;
                } else if (c == '/')
                {
                    if (i + 1 < line.size() && (line[i + 1] == '*' || line[i + 1] == '/'))
                    {
                        getOneToken(COMMENT_BEGIN);
                    } else
                    {
                        getOneToken(IDENTIFIER);
                        value = c;
                    }
                } else if (isSpecial(c))
                {
                    getOneToken(SPECIAL_CHAR);
                    value = c;
                } else
                {
                    getOneToken(IDENTIFIER);
                    value = c;
                }
            } else if (token == ORDER_NUM)
            {
                if (isblank(c))
                {
                    getOneToken(INIT);
                } else if (isdigit(c))
                {
                    value += c;
                } else if (c == '/')
                {
                    if (i + 1 < line.size() && (line[i + 1] == '*' || line[i + 1] == '/'))
                    {
                        getOneToken(COMMENT_BEGIN);
                    } else
                    {
                        getOneToken(IDENTIFIER);
                        value = c;
                    }
                } else if (isSpecial(c))
                {
                    getOneToken(SPECIAL_CHAR);
                    value = c;
                } else
                {
                    getOneToken(IDENTIFIER);
                    value = c;
                }
            } else if (token == COMMENT_BEGIN)
            {
                if (c == '*')
                {
                    token = COMMENT_STAR;
                } else if (c == '/')
                {
                    token = COMMENT_DOUBLE_SLASH;
                } else
                {
                    assert(0);
                }
            } else if (token == COMMENT_STAR)
            {
                if (c == '*' && i + 1 < line.size() && line[i + 1] == '/')
                {  // 尝试看下一个字符
                    i++;                                                        // 跳过'/'
                    token = INIT;                                         // 解析状态变成初始化
                }
            } else if (token == COMMENT_DOUBLE_SLASH)
            {
                if (i == line.size() - 1)
                {  // 双斜线的注释范围到行尾结束
                    token = INIT;        // 解析状态变成初始化
                }
            } else
            {
                assert(0);
            }
        }
    }
}

