#pragma once

#include <cstring>
#include "common/common.h"
#include "define.h"
#include "request_parse.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class Request
    {
    private:
        Function<size_t(char *, size_t)> readFun;
        Header _header;
        Header _query;
        SortMap<String, String> _params;
        FormData formData;
        // 预读body
        const char *_body{nullptr};
        // 未读取长度
        size_t _length{0};

    public:
        HttpMethod method{HttpMethod::HTTP_GET};
        String path;
        String version;
        String queryRow;

    public:
        explicit Request() = default;

        explicit Request(Function<size_t(char *, int)> read,const char *body, size_t length) : readFun(std::move(read)),
                                                                                               _body(body), _length(length)
        {}

        [[nodiscard]] String getParam(const String &key) const;

        [[nodiscard]] String getQuery(const String &key) const;

        [[nodiscard]] StringList getQuerys(const String &key) const;

        [[nodiscard]] String getHeader(const String &key) const;

        [[nodiscard]] StringList getHeaders(const String &key) const;

        [[nodiscard]] Header getHeader() const;

        [[nodiscard]] Header getQuery() const;

        void setQuery(Header query);

        void setParams(SortMap<String, String> params);

        void setHeader(Header query);

        size_t readBody(char *data, size_t length);

        [[nodiscard]] size_t getContentLength() const;
    };
}