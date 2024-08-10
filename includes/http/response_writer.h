#pragma once

#include <sstream>
#include <cstring>
#ifdef OS_LINUX
#include <sys/sendfile.h>
#endif
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
        std::stringstream _buf{};
        bool _done{false};

    public:
        explicit HttpResponseWriter() = default;

        explicit HttpResponseWriter(Function<size_t(char *, size_t)> write) : _write(std::move(write))
        {}

        ~HttpResponseWriter();

        void setStatus(HttpStatus status);

        [[nodiscard]] HttpStatus getStatus() const;

        void setHeader(const String &key, const Vector<String> &values);

        void addHeader(const String &key, const String &value);

        void setContentType(const String &value);

        void write(char *data, size_t length);

        void write(const String &data);

        off_t sendfile(int fd, const std::string &contentType, int file_fd, size_t count);

    private:
        void writeRespLine() const;

        void writeRespHeader() const;

        void writeWithKV(const String &key, const String &value) const;

        void writeRespBody() const;
    };
}