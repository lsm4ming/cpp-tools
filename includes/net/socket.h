#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <utility>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class Socket
    {
    private:
        int _fd{-1};
        bool _connected{false};
        String _host;
        uint16 _port{0};

    public:
        Socket() = default;
        ~Socket();
        Socket(String host, uint16 port) : _host(std::move(host)), _port(port){};
        Socket(const Socket &other) = delete;
        Socket(Socket &&other) = delete;
        Socket &operator=(Socket &&other) = delete;

        void setHost(const String &host);
        [[nodiscard]] String getHost() const;
        void setPort(uint16 port);
        [[nodiscard]] uint16 getPort() const;

        int connect();
        int connect(const String &host, uint16 port);
        [[nodiscard]] int setBlocking(bool blocking) const;
        [[nodiscard]] int setKeepalive(bool keepalive) const;
        [[nodiscard]] int setReuseaddr(bool reuseaddr) const;
        [[nodiscard]] int setReuseport(bool reuseport) const;
        [[nodiscard]] int setTcpNoDelay(bool tcpNoDelay) const;

        ssize_t send(char* data, uint32 length) const;
        ssize_t receive(char* data, uint32 length) const;

        void close();
    };
}