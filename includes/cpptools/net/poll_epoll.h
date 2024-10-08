#pragma once

#include <cstring>
#include "cpptools/common/common.h"
#include "cpptools/log/log.h"
#include "poll_event.h"
#include "poll.h"
#include "poll_handler.h"
#ifdef OS_LINUX
#include <sys/epoll.h>

namespace cpptools::net
{
    class PollEpoll : public PollEvent
    {
    private:
        int socket_fd{-1};
        int epoll_fd{-1};
        ChannelHandler *_handler;
        HashMap<int, Channel> channelMap{};

    public:
        explicit PollEpoll(ChannelHandler *handler);

        int makeup(int fd) override;

        void close() override;

        int pollWait(int timeout) override;
    };
}
#endif