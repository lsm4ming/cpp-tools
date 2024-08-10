#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class InetAddress
    {
    private:
        sockaddr_in addr_{};
    public:
        InetAddress() = default;

        InetAddress(const String &ip, uint16_t port);

        [[maybe_unused]] explicit InetAddress(const sockaddr_in& addr);

        ~InetAddress() = default;

        [[nodiscard]] const char *ip() const;

        [[nodiscard]] uint16_t port() const;

        [[nodiscard]] const sockaddr *addr() const;

        void setAddr(sockaddr_in clientAddr);
    };
}