#pragma once

#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::http
{
    class HttpServer
    {
    private:
        String _host{"127.0.0.1"};
        uint16 _port{8080};
        int fd{-1};
        String _staticDir;

    public:
        explicit HttpServer() = default;

        ~HttpServer() = default;

        HttpServer(const HttpServer &) = delete;

        HttpServer(HttpServer &&) = delete;

        HttpServer &operator=(const HttpServer &) = delete;

        HttpServer &operator=(HttpServer &&) = delete;

        void setHost(const String &host);

        void setPort(uint16 port);

        void setStaticDir(const String &dir);

        void start();

        void stop();
    };
}