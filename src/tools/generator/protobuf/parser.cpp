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
            if (!value.empty() && !isSkip(value))
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

    void ProtobufParser::parse2ServiceInfo(Vector<Tuple<Token, String>> &tokens, ServiceInfo &serviceInfo)
    {
        for (size_t i = 0; i < tokens.size(); i++)
        {
            auto &token = tokens[i];
            if (std::get<1>(token) == "package" && i + 1 < tokens.size()) // 赋值包名
            {
                serviceInfo.package_name_ = std::get<1>(tokens[i + 1]);
                continue;
            }
            if (std::get<1>(token) == "service" && i + 1 < tokens.size())
            {
                serviceInfo.service_name_ = std::get<1>(tokens[i + 1]);
                continue;
            }
            if (std::get<1>(token) == "rpc" && i + 7 < tokens.size())
            {
                RpcInfo rpcInfo;
                rpcInfo.rpc_mode_ = REQ_RESP;  // 默认是REQ_RESP模式
                rpcInfo.rpc_name_ = std::get<1>(tokens[i + 1]);
                rpcInfo.request_name_ = std::get<1>(tokens[i + 3]);
                rpcInfo.response_name_ = std::get<1>(tokens[i + 7]);
                rpcInfo.out_file_name_ = rpcInfo.rpc_name_ + ".cpp";
                if (i + 15 < tokens.size() && std::get<1>(tokens[i + 12]) == "MySvr.Base.MethodMode")
                {
                    if (std::get<1>(tokens[i + 15]) == "2")
                    {
                        rpcInfo.rpc_mode_ = ONE_WAY;
                    }
                    if (std::get<1>(tokens[i + 15]) == "3")
                    {
                        rpcInfo.rpc_mode_ = FAST_RESP;
                    }
                }
                serviceInfo.rpc_infos_.push_back(rpcInfo);
                continue;
            }
            if (std::get<1>(token) == "message" && i + 1 < tokens.size())
            {
                // 解析结构体
                i++;
                parseMessageInfo(tokens, i, serviceInfo);
                continue;
            }
        }
    }

    bool ProtobufParser::isSkip(char c)
    {
        // 如果是换行符则直接跳过
        return c == '\r' || c == '\n';
    }

    void ProtobufParser::parseMessageInfo(Vector<Tuple<Token, String>> &tokens, size_t &i, ServiceInfo &info)
    {
        RpcMessage message;
        message.name_ = std::get<1>(tokens[i]);
        // 过滤{
        while (std::get<1>(tokens[++i]) == "{");
        // 解析结构体
        while (true)
        {
            if (std::get<1>(tokens[i]) == "}")
            {
                break;
            } else if (std::get<1>(tokens[i]) == ";")
            {
                ++i;
                continue;
            }
            auto field = RpcMessageField{};
            auto temp = std::get<1>(tokens[i]);
            if (temp == "optional")
            {
                field.flag |= 0b1;
            }
            if (temp == "required")
            {
                field.flag |= 0b01;
            }
            if (temp == "repeated")
            {
                field.flag |= 0b001;
            }
            if (field.flag == 0)
            {
                field.type_ = convertMessageFieldType(temp);
            } else
            {
                field.type_ = convertMessageFieldType(std::get<1>(tokens[++i]));
            }
            field.name_ = std::get<1>(tokens[++i]);
            // 去除=
            ++i;
            field.no_ = std::stoi(std::get<1>(tokens[++i]));
            // 去除;
            ++i;
            message.fields_.push_back(field);
        }
        info.messages_[message.name_] = message;
    }

    MessageType ProtobufParser::convertMessageFieldType(const String &type)
    {
        if (type == "int32")
            return MESSAGE_TYPE_INT32;
        if (type == "int64")
            return MESSAGE_TYPE_INT64;
        if (type == "string")
            return MESSAGE_TYPE_STRING;
        if (type == "double")
            return MESSAGE_TYPE_DOUBLE;
        if (type == "float")
            return MESSAGE_TYPE_FLOAT;
        if (type == "bool")
            return MESSAGE_TYPE_BOOL;
        if (type == "uint32")
            return MESSAGE_TYPE_UINT32;
        if (type == "uint64")
            return MESSAGE_TYPE_UINT64;
        if (type == "bytes")
            return MESSAGE_TYPE_BYTES;
        if (type == "enum")
            return MESSAGE_TYPE_ENUM;
        if (type == "map")
            return MESSAGE_TYPE_MAP;
        if (type == "string")
            return MESSAGE_TYPE_STRING;
        return MESSAGE_TYPE_MESSAGE;
    }

    bool ProtobufParser::isSkip(String &c)
    {
        // 去除前面的换行符
        c = cpptools::utils::Strings::ltrim(c, "\r");
        c = cpptools::utils::Strings::ltrim(c, "\n");
        c = cpptools::utils::Strings::ltrim(c, "\t");
        return c.empty();
    }
}