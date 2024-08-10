#pragma once

#include "cpptools/common/common.h"

#ifdef OS_MAC
#include <sys/event.h>
#include <unistd.h>
#include <memory>
#include "poll_channel.h"
#include "poll_event.h"

using namespace cpptools::common;

constexpr int kReadEvent = 1;
constexpr int kWriteEvent = 2;

namespace cpptools::net
{
    class PollKqueue : public PollEvent
    {
    private:
        int socket_fd{-1};
        int kqueue_fd{-1};
        ChannelHandler *_handler;

    public:
        PollKqueue() = default;

        explicit PollKqueue(ChannelHandler *_handler) : _handler(_handler)
        {}

        int makeup(int fd) override;

        void close() override;

        int pollWait(int timeout) override;

        int updateChannel(Channel *ch) override;

        int addChannel(Channel *ch) override;

        int removeChannel(Channel *ch) override;
    };
}
#endif
