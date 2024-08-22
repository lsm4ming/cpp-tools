#pragma once

#include <fstream>
#include <cassert>
#include "cpptools/common/common.h"

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

    typedef struct RpcInfo
    {
        String rpc_name_; // RPC 名称
        RpcMode rpc_mode_; // RPC 模式
        String request_name_; // Request 名称
        String response_name_; // Response 名称
        String out_file_name_; // 输出文件名
    } RpcInfo;

    typedef struct ServiceInfo
    {
        String package_name_; // 包名
        String service_name_; // 服务名
        String cpp_namespace_name_; //
        String handler_file_prefix_; //
        String port_; // 端口
        std::vector<RpcInfo> rpc_infos_; // RPC 信息
    } ServiceInfo;

    static bool isSpecial(char c)
    {
        return c == '=' || c == ';' || c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']';
    }

    class ProtobufParser
    {
    public:
        static Vector<Tuple<Token, String>> parse2Token(const String &path);

    private:
        static void parseLine(const String &line, String &value, Token &token, Vector<Tuple<Token, String>> &tokens);
    };
}

