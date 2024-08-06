#include "net/server_socket.h"

namespace cpptools::net
{
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