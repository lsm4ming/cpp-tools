#pragma once

#include "common/common.h"
#include "define.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class HttpResponseWriter
    {
    private:
        int _fd{-1};
        HttpStatus _status{HttpStatus::HTTP_OK};
        Header _headers;

    public:
        explicit HttpResponseWriter() = default;

        void setStatus(HttpStatus status);

        [[nodiscard]] HttpStatus getStatus() const;

        void setHeader(const String &key, const Vector<String> &values);

        void addHeader(const String &key, const String &value);

        void setContentType(const String &value);

        size_t write(char *data, size_t length) const;
    };
}