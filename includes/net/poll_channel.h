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

    public:
        Channel() = default;

        ~Channel() = default;

        Channel(int fd, uint32_t events, epoll_data_t data) : events(events), data(data)
        {
            this->data.fd = fd;
        };

        void enableReading();

        void enableWriting();

        void disableAll();

        void enableEt();

        void enableAll();

        void enableNoBlocking() const;

        [[nodiscard]] int getFd() const;
    };
}