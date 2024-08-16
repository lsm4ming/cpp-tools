#pragma once

#include <cassert>
#include <fcntl.h>
#include <sys/sysinfo.h>

#include <functional>

#include "codec.hpp"
#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::reactor
{
    void SetNotBlock(int fd)
    {
        int oldOpt = fcntl(fd, F_GETFL);
        assert(oldOpt != -1);
        assert(fcntl(fd, F_SETFL, oldOpt | O_NONBLOCK) != -1);
    }

    void SetTimeOut(int fd, int64_t sec, int64_t usec)
    {
        struct timeval tv{};
        tv.tv_sec = sec;    //秒
        tv.tv_usec = usec;  //微秒，1秒等于10的6次方微秒
        assert(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != -1);
        assert(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) != -1);
    }

    int CreateListenSocket(const String &host, uint16 port, bool isReusePort)
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        int sockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockFd < 0)
        {
            perror("socket failed");
            return -1;
        }
        int reuse = 1;
        int opt = SO_REUSEADDR;
        if (isReusePort) opt = SO_REUSEPORT;
        if (setsockopt(sockFd, SOL_SOCKET, opt, &reuse, sizeof(reuse)) != 0)
        {
            perror("setsockopt failed");
            return -1;
        }
        if (bind(sockFd, (sockaddr *) &addr, sizeof(addr)) != 0)
        {
            perror("bind failed");
            return -1;
        }
        if (listen(sockFd, 1024) != 0)
        {
            perror("listen failed");
            return -1;
        }
        return sockFd;
    }

    // 调用本函数之前需要把sockFd设置成非阻塞的
    void LoopAccept(int sockFd, int maxConn, const std::function<void(int clientFd)>& clientAcceptCallBack)
    {
        while (maxConn--)
        {
            int clientFd = accept(sockFd, nullptr, nullptr);
            if (clientFd > 0)
            {
                clientAcceptCallBack(clientFd);
                continue;
            }
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
            {
                perror("accept failed");
            }
            break;
        }
    }
}  // namespace EchoServer