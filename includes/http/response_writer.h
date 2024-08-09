#pragma once

#include "common/common.h"
#include "define.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class HttpResponseWriter
    {
    private:
        HttpStatus _status{HttpStatus::HTTP_OK};
        Header _headers;
        Function<size_t(char *, size_t)> _write;

    public:
        explicit HttpResponseWriter() = default;

        explicit HttpResponseWriter(Function<size_t(char *, size_t)> write) : _write(std::move(write))
        {}

        void setStatus(HttpStatus status);

        [[nodiscard]] HttpStatus getStatus() const;

        void setHeader(const String &key, const Vector<String> &values);

        void addHeader(const String &key, const String &value);

        void setContentType(const String &value);

        size_t write(char *data, size_t length) const;
    };
}