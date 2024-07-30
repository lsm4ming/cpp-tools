#pragma once

#include "define.h"
#include "httpresponse.h"

namespace cpptools::http
{
    class HttpRequest
    {
        friend HttpClient;

    private:
        // 协议
        String _protocol;
        String _domain;
        String _path;
        String _queryRaw;
        String _rawUrl;
        HttpMethod _method{HttpMethod::HTTP_GET};
        Header _header;
        Header _query;
        char *_body{};
        size_t _length{};
        int _fd{};

    public:
        HttpRequest() = default;
        HttpRequest(HttpMethod method, String url) : _method(method), _rawUrl(std::move(url)) {}
        explicit HttpRequest(String url) : _rawUrl(std::move(url)) {}
        ~HttpRequest() = default;

        void setQuery(const String &key, const String &value);
        void addQuery(const String &key, const String &value);
        void setQuery(const String &key, const Vector<String> &values);
        void addQuery(const String &key, const Vector<String> &values);
        void setHeader(const String &key, const String &value);
        void addHeader(const String &key, const String &value);
        void setHeader(const String &key, const Vector<String> &values);
        void addHeader(const String &key, const Vector<String> &values);
        void setBody(char *body, size_t length);
        void setTimeout(int seconds);
        HttpResponse post(const String &url);
        HttpResponse get(const String &url);
        HttpResponse doSend(const HttpMethod &method, const String &url);

    private:
        static String host_to_ip(const String &hostname);

        static String joinValues(const Vector<String> &values);

        static void set_socket_timeout(int fd, int seconds);

        String assembleUrl();

        String encodeQueryParameters();

        static int http_create_socket(const String &ip);

        static Header parseQuery(const String &queryRaw);

        HttpResponse send();
    };
}