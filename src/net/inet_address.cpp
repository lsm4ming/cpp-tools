#include "net/inet_address.h"

namespace cpptools::net
{
    InetAddress::InetAddress(const String &ip, uint16_t port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    [[maybe_unused]] InetAddress::InetAddress(const sockaddr_in addr)
    {
        addr_ = addr;
    }

    const char *InetAddress::ip() const
    {
        return inet_ntoa(addr_.sin_addr);
    }

    uint16_t InetAddress::port() const
    {
        return ntohs(addr_.sin_port);
    }

    const sockaddr *InetAddress::addr() const
    {
        return (const sockaddr *)&addr_;
    }

    void InetAddress::setAddr(sockaddr_in clientAddr)
    {
        addr_ = clientAddr;
    }
}