#include "net/server_socket.h"

#include <utility>

namespace cpptools::net
{
    ServerSocket::ServerSocket(String host, uint16 port) : Socket(std::move(host), port)
    {
        this->_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->_fd < 0)
        {
            throw std::runtime_error("socket error");
        }
    }

    int ServerSocket::bind()
    {
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(getPort());
        addr.sin_addr.s_addr = inet_addr(getHost().c_str());
        return ::bind(this->getFd(), (struct sockaddr *) &addr, sizeof(sockaddr));
    }

    int ServerSocket::listen(int n)
    {
        return ::listen(this->getFd(), n);
    }

    int ServerSocket::accept(InetAddress &address)
    {
        sockaddr_in peerAddr{};
        socklen_t len = sizeof(peerAddr);
        int client_fd = ::accept(getFd(), (sockaddr *) &peerAddr, &len);
        address.setAddr(peerAddr);
        return client_fd;
    }
}