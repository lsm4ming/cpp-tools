#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
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
        Socket(const String &host, uint16 port) : _host(host), _port(port){};
        Socket(const Socket &other) = delete;
        Socket(Socket &&other) = delete;
        Socket &operator=(Socket &&other) = delete;

        void setHost(const String &host);
        String getHost() const;
        void setPort(uint16 port);
        uint16 getPort() const;

        int connect();
        int connect(const String &host, uint16 port);
        int setBlocking(bool blocking);
        int send(char* data, uint32 length);
        int receive(char* data, uint32 length);
        void close();
    };
}