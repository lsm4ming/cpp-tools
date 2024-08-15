#pragma once

#include <cstring>
#include <sys/stat.h>
#include <iostream>
#include "cpptools/log/log.h"
#include "cpptools/common/common.h"
#include "cpptools/net/poll_event.h"
#include "cpptools/net/server_socket.h"
#include "http_router.h"
#include "define.h"
#include "request_parse.h"

using namespace cpptools::common;
using namespace cpptools::net;

namespace cpptools::http
{
    class HttpServer;

    class HttpProtocolHandler : public ConnectHandler
    {
    private:
        HttpServer *pServer;

    public:
        explicit HttpProtocolHandler(HttpServer *pServer);

        void onAccept(const PollConn &conn) override;

        void onRead(const PollConn &conn) override;

        void onWrite(const PollConn &conn) override;

        void onClose(const PollConn &conn) override;
    };

    class HttpServer
    {
        friend class HttpProtocolHandler;

    private:
        String _host{"127.0.0.1"};
        uint16 _port{8080};
        String _staticDir;
        volatile std::atomic_bool running{false};
        HttpRouter router;

    private:
        void dispatch(int fd, Request &request, HttpResponseWriter &response);

        bool staticDispatch(int fd, Request &request, HttpResponseWriter &response);

    public:
        explicit HttpServer() = default;

        explicit HttpServer(String host, uint16 port) : _host(std::move(host)), _port(port)
        {}

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

        void addRoute(HttpMethod method, const std::string &path, const RouteHandler &handler);
    };
}