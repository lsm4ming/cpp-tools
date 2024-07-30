#pragma once

#include <regex>
#include <arpa/inet.h>
#include <netdb.h>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class HttpRequest;

    class HttpResponse;

    class HttpClient;

    class HttpServer;

    inline std::regex url_regex(R"((https?)://([^/\r\n]+)(/[^\r\n?]*)?(\?[^#\r\n]*)?)");
    inline std::regex resp_line_regex;

    constexpr const int BUFFER_SIZE = 4096;
    constexpr const char *HTTP_VERSION = "HTTP/1.1";
    constexpr const char *WRAP = "\r\n";
    constexpr const char *EMPTY = " ";

    using Header = SortMap <String, Vector<String>>;

    enum HttpStatus
    {
        // 继续（说明收到了请求的初始部分，请客户端继续）
        HTTP_CONTINUE = 100,
        // 切换协议
        HTTP_SWITCHING_PROTOCOLS = 101,
        // OK
        HTTP_OK = 200,
        // 创建
        HTTP_CREATED = 201,
        // 已接受
        HTTP_ACCEPTED = 202,
        // 部分更新
        HTTP_NON_AUTHORITATIVE_INFORMATION = 203,
        // 无内容
        HTTP_NO_CONTENT = 204,
        // 重置内容
        HTTP_RESET_CONTENT = 205,
        // 部分内容
        HTTP_PARTIAL_CONTENT = 206,
        // 重定向，请求的资源现在临时从不同的URI响应
        HTTP_MULTIPLE_CHOICES = 300,
        // 重定向，请求的资源已被永久移动到新位置
        HTTP_MOVED_PERMANENTLY = 301,
        // 临时重定向
        HTTP_TEMPORARY_REDIRECT = 302,
        // 未修改
        HTTP_NOT_MODIFIED = 304,
        // 客户端错误
        HTTP_BAD_REQUEST = 400,
        // 未授权
        HTTP_UNAUTHORIZED = 401,
        // 客户端错误 类似于401，但进入403状态后即使重新验证也不会改变该状态，该访问是长期禁止的
        HTTP_FORBIDDEN = 403,
        // 资源不存在
        HTTP_NOT_FOUND = 404,
        // 方法不允许
        HTTP_METHOD_NOT_ALLOWED = 405,
        // 不接受
        HTTP_NOT_ACCEPTABLE = 406,
        // 请求超时
        HTTP_REQUEST_TIMEOUT = 408,
        // 资源冲突
        HTTP_CONFLICT = 409,
        // 资源移动
        HTTP_GONE = 410,
        // Content-Length长度必需
        HTTP_LENGTH_REQUIRED = 411,
        // 服务器错误
        HTTP_INTERNAL_SERVER_ERROR = 500,
        // 未实现
        HTTP_NOT_IMPLEMENTED = 501,
        // 网关错误
        HTTP_BAD_GATEWAY = 502,
        // 服务不可用
        HTTP_SERVICE_UNAVAILABLE = 503,
        // 网关超时
        HTTP_GATEWAY_TIMEOUT = 504,
        // 协议错误
        HTTP_PROTOCOL_ERROR = 505
    };

    enum HttpMethod
    {
        // GET
        HTTP_GET = 0,
        // POST
        HTTP_POST = 1,
        // HEAD
        HTTP_HEAD = 2,
        // PUT
        HTTP_PUT = 3,
        // DELETE
        HTTP_DELETE = 4,
        // PATCH
        HTTP_PATCH = 5,
        // OPTIONS
        HTTP_OPTIONS = 6
    };

    class HttpStatusException : public std::exception
    {
    public:
        explicit HttpStatusException(HttpStatus status) : status_(status)
        {}

        [[nodiscard]] HttpStatus status() const
        { return status_; }

    private:
        HttpStatus status_;
    };

    inline HashMap <HttpStatus, String> statusMap =
            {
                    {HTTP_CONTINUE,                      "Continue"},
                    {HTTP_SWITCHING_PROTOCOLS,           "Switching Protocols"},
                    {HTTP_OK,                            "OK"},
                    {HTTP_CREATED,                       "Created"},
                    {HTTP_ACCEPTED,                      "Accepted"},
                    {HTTP_NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information"},
                    {HTTP_NO_CONTENT,                    "No Content"},
                    {HTTP_RESET_CONTENT,                 "Reset Content"},
                    {HTTP_PARTIAL_CONTENT,               "Partial Content"},
                    {HTTP_MULTIPLE_CHOICES,              "Multiple Choices"},
                    {HTTP_MOVED_PERMANENTLY,             "Moved Permanently"},
                    {HTTP_TEMPORARY_REDIRECT,            "Temporary Redirect"},
                    {HTTP_NOT_MODIFIED,                  "Not Modified"},
                    {HTTP_BAD_REQUEST,                   "Bad Request"},
                    {HTTP_UNAUTHORIZED,                  "Unauthorized"},
                    {HTTP_FORBIDDEN,                     "Forbidden"},
                    {HTTP_NOT_FOUND,                     "Not Found"},
                    {HTTP_METHOD_NOT_ALLOWED,            "Method Not Allowed"},
                    {HTTP_NOT_ACCEPTABLE,                "Not Acceptable"},
                    {HTTP_REQUEST_TIMEOUT,               "Request Timeout"},
                    {HTTP_CONFLICT,                      "Conflict"},
                    {HTTP_GONE,                          "Gone"},
                    {HTTP_LENGTH_REQUIRED,               "Length Required"},
                    {HTTP_INTERNAL_SERVER_ERROR,         "Internal Server Error"},
                    {HTTP_NOT_IMPLEMENTED,               "Not Implemented"},
                    {HTTP_BAD_GATEWAY,                   "Bad Gateway"},
                    {HTTP_SERVICE_UNAVAILABLE,           "Service Unavailable"},
                    {HTTP_GATEWAY_TIMEOUT,               "Gateway Timeout"},
                    {HTTP_PROTOCOL_ERROR,                "Protocol Error"},
            };

    inline HashMap <HttpMethod, String> methodMap = {
            {HTTP_GET, "GET"},
            {HTTP_POST, "POST"},
            {HTTP_HEAD, "HEAD"},
            {HTTP_PUT, "PUT"},
            {HTTP_DELETE, "DELETE"},
            {HTTP_PATCH, "PATCH"},
            {HTTP_OPTIONS, "OPTIONS"},
    };

    static String methodToString(HttpMethod method)
    {
        return methodMap[method];
    }

    static String statusToString(HttpStatus status)
    {
        return statusMap[status];
    }
}