#pragma once

#include <utility>

#include "common/common.h"
#include "define.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class Request
    {
    private:
        Function<size_t(char *, size_t)> read;
        String _queryRow;
        Header header;
        Header query;
        HashMap<String, String> params;
        FormData formData;

    public:
        HttpMethod method{HttpMethod::HTTP_GET};
        String url;
        String version;

    public:
        explicit Request(Function<size_t(char *, int)> read) : read(std::move(read))
        {}

        Request(Function<size_t(char *, size_t)> read, HttpMethod method, String url, String version) : read(std::move(read)), method(method), url(std::move(url)),
                                                                         version(std::move(version))
        {}

        String getParams(const String &key) const;

        String getQuery(const String &key) const;

        StringList getQuerys(const String &key) const;

        String getHeader(const String &key) const;

        StringList getHeaders(const String &key) const;
    };
}