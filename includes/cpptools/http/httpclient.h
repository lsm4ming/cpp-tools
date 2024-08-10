#pragma once

#include "httprequest.h"
#include "httpresponse.h"

namespace cpptools::http
{
    class HttpClient
    {
    private:
        HttpRequest _request;

    public:
        explicit HttpClient() = default;
        ~HttpClient() = default;
        HttpClient *setQuery(const String &key, const String &value);
        HttpClient *addQuery(const String &key, const String &value);
        HttpClient *setQuery(const String &key, const Vector<String> &values);
        HttpClient *addQuery(const String &key, const Vector<String> &values);
        HttpClient *setHeader(const String &key, const String &value);
        HttpClient *addHeader(const String &key, const String &value);
        HttpClient *setHeader(const String &key, const Vector<String> &values);
        HttpClient *addHeader(const String &key, const Vector<String> &values);
        HttpClient *setBody(char *body, size_t length);
        HttpClient *setTimeout(int seconds);
        HttpResponse post(const String &url);
        HttpResponse get(const String &url);
        HttpResponse put(const String &url);
        HttpResponse delete_(const String &url);
        HttpResponse head(const String &url);
        HttpResponse options(const String &url);
        HttpResponse patch(const String &url);
        HttpResponse doSend(const HttpMethod &method, const String &url);
        HttpResponse doSend(const HttpRequest &request);
    };
}