#pragma once

#include <cstring>
#include "define.h"

namespace cpptools::http
{
    class HttpResponse
    {
        friend HttpRequest;

    private:
        int _code{};
        String _status;
        String _body;
        size_t _length{};
        Header _header;

    public:
        [[nodiscard]] int statusCode() const;

        [[nodiscard]] String httpStatus() const;

        [[nodiscard]] String getBody() const;

    private:
        void readResponse(int fd);
    };
}