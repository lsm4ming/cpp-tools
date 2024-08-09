#pragma once

#include <utility>

#include "common/common.h"
#include "define.h"
#include "request_parse.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class Request
    {
    private:
        Function<size_t(char *, size_t)> read;
        Header _header;
        Header _query;
        SortMap<String, String> params;
        FormData formData;

    public:
        HttpMethod method{HttpMethod::HTTP_GET};
        String url;
        String version;
        String queryRow;

    public:
        explicit Request() = default;

        explicit Request(Function<size_t(char *, int)> read) : read(std::move(read))
        {}

        Request(Function<size_t(char *, size_t)> read, HttpMethod method, String url, String version) : read(std::move(read)), method(method), url(std::move(url)),
                                                                         version(std::move(version))
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
    };
}