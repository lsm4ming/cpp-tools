#pragma once

#include "define.h"

namespace cpptools::http
{
    class HttpResponse
    {
        friend HttpRequest;

    private:
        int _code{};
        String _status;
        String body;
        size_t _length{};
        Header _header;

    public:
        int statusCode() const;

        String httpStatus() const;

        String getBody() const;

    private:
        void readResponse(int fd);
    };
}