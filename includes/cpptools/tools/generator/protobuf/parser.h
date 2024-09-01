#pragma once

#include <fstream>
#include <cassert>
#include "cpptools/common/common.h"
#include "cpptools/utils/strings.h"

namespace cpptools::tools::generator::protobuf
{
    using namespace cpptools::common;

    enum Token
    {
        INIT = 0,           // 初始化状态
        IDENTIFIER = 1,     // 标识符（包括关键字）-> syntax "proto3" package MySvr.Auth message GenTicketRequest
        SPECIAL_CHAR = 2,   // 特殊字符 -> = ; { } ( ) [ ]
        ORDER_NUM = 3,      // 序号 -> 1 2 3
        COMMENT_BEGIN = 4,  // 注释模式
        COMMENT_STAR = 5,   // 注释模式1 -> /*
        COMMENT_DOUBLE_SLASH = 6,  // 注释模式2 -> //
    };

    enum RpcMode
    {
        REQ_RESP = 1,   // Request-Response
        ONE_WAY = 2,    // Oneway
        FAST_RESP = 3,  // Fast-Response
    };

    enum MessageType
    {
        MESSAGE_TYPE_INT32 = 1,
        MESSAGE_TYPE_INT64 = 2,
        MESSAGE_TYPE_UINT32 = 3,
        MESSAGE_TYPE_UINT64 = 4,
        MESSAGE_TYPE_FLOAT = 5,
        MESSAGE_TYPE_DOUBLE = 6,
        MESSAGE_TYPE_BOOL = 7,
        MESSAGE_TYPE_STRING = 8,
        MESSAGE_TYPE_BYTES = 9,
        MESSAGE_TYPE_ENUM = 10,
        MESSAGE_TYPE_MAP = 11,
        MESSAGE_TYPE_MESSAGE = 12,
    };

    typedef struct RpcInfo
    {
        String rpc_name_; // RPC 名称
        RpcMode rpc_mode_; // RPC 模式
        String request_name_; // Request 名称
        String response_name_; // Response 名称
        String out_file_name_; // 输出文件名
    } RpcInfo;

    typedef struct RpcMessageField
    {
        String name_; // 字段名称
        MessageType type_; // 字段类型
        uint32 no_; // 字段序号
        uint8_t flag; // 字段标记 依次为 optional required repeated
    } RpcMessageField;

    typedef struct RpcMessage
    {
        String name_; // 消息名称
        Vector<RpcMessageField> fields_; // 字段
    } RpcMessage;

    typedef struct ServiceInfo
    {
        String package_name_; // 包名
        String service_name_; // 服务名
        String cpp_namespace_name_; //
        String handler_file_prefix_; //
        String port_; // 端口
        Vector<RpcInfo> rpc_infos_; // RPC 信息
        SortMap<String, RpcMessage> messages_;
    } ServiceInfo;

    static bool isSpecial(char c)
    {
        return c == '=' || c == ';' || c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']';
    }

    class ProtobufParser
    {
    public:
        static Vector<Tuple<Token, String>> parse2Token(const String &path);

        static void parse2ServiceInfo(Vector<Tuple<Token, String>> &tokens, ServiceInfo &serviceInfo);

    private:
        static void parseLine(const String &line, String &value, Token &token, Vector<Tuple<Token, String>> &tokens);

        static bool isSkip(char c);

        static bool isSkip(String &c);

        static void parseMessageInfo(Vector<Tuple<Token, String>> &tokens, size_t &i, ServiceInfo &info);

        static MessageType convertMessageFieldType(const String &type);
    };
}

