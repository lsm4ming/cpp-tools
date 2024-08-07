#pragma once

#include <sys/epoll.h>
#include "common/common.h"
#include "poll_event.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class Channel
    {
    public:
        uint32_t events{};
        epoll_data_t data{};

    protected:
        int _fd{};

    public:
        Channel() = default;

        ~Channel() = default;

        void enableReading();

        int getFd() const;
    };
}