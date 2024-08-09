#pragma once

#include "common/common.h"
#include "server_request.h"
#include "define.h"
#include "utils/string.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class RequestParse
    {
    private:
        String raw;

    public:
        explicit RequestParse(String raw)
        {
            this->raw = std::move(raw);
        }

        int parse(Request &request);

    private:
        int parseRequestLine(Request &request);

        int parseRequestHeader(Request &request);
    };
}