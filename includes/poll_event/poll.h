#pragma once

#include <sys/epoll.h>
#include "common/common.h"
#include "channel.h"

using namespace cpptools::common;

namespace cpptools::pollEvent
{
    constexpr const int MaxEvents = 100; // epoll_wait()返回事件数组的大小。

    class Poll
    {
    private:
        int poll_fd{-1};
        epoll_event events_[MaxEvents]{};

    public:
        explicit Poll();

        ~Poll();

        int updateChannel(Channel *ch) const;

        int removeChannel(Channel *ch) const;

        Vector<Channel *> loop(int timeout = -1);
    };
}

