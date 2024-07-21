#include "net/socket.h"

cpptools::net::Socket::~Socket()
{
    this->close();
}

void cpptools::net::Socket::setHost(const String &host)
{
    this->_host = host;
}

String cpptools::net::Socket::getHost() const
{
    return this->_host;
}

void cpptools::net::Socket::setPort(uint16 port)
{
    this->_port = port;
}

uint16 cpptools::net::Socket::getPort() const
{
    return this->_port;
}

int cpptools::net::Socket::connect()
{
    return this->connect(this->_host, this->_port);
}

int cpptools::net::Socket::connect(const String &host, uint16 port)
{
    if (this->_connected) // 已经打开连接了
    {
        return this->_fd;
    }
    this->_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_fd < 0)
    {
        return -1;
    }
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    if (::connect(this->_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        ::close(this->_fd);
        return -1;
    }
    this->_connected = true;
    return this->_fd;
}

int cpptools::net::Socket::setBlocking(bool blocking) const
{
    int flags = fcntl(this->_fd, F_GETFL, 0);
    if (blocking)
    {
        return fcntl(this->_fd, F_SETFL, flags & ~O_NONBLOCK);
    }
    return fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK);
}

int cpptools::net::Socket::send(char *data, uint32 length)
{
    if (this->_connected)
    {
        return ::send(this->_fd, data, length, 0);
    }
    return -1;
}

int cpptools::net::Socket::receive(char *data, uint32 length) const
{
    if (this->_connected)
    {
        return ::recv(this->_fd, data, length, 0);
    }
    return -1;
}

void cpptools::net::Socket::close()
{
    if (this->_fd > -1)
    {
        ::close(this->_fd);
        this->_connected = false;
        this->_fd = -1;
    }
}
