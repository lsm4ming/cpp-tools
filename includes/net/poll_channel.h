#pragma once

#include <fcntl.h>
#include "common/common.h"

#ifdef OS_MAC

#include <sys/event.h>

#elif OS_LINUX
#include <sys/epoll.h>
#endif

#include "poll_event.h"

using namespace cpptools::common;

namespace cpptools::net
{
#ifdef OS_MAC
    constexpr const int EVENT_READ = EVFILT_READ;
    constexpr const int EVENT_WRITE = EVFILT_WRITE;
    constexpr const int EVENT_ET = EV_CLEAR;
    constexpr const int EVENT_ALL = EVENT_READ | EVENT_WRITE | EVENT_ET;
#elif OS_LINUX
    constexpr const int EVENT_READ = EPOLLIN;
    constexpr const int EVENT_WRITE = EPOLLOUT;
    constexpr const int EVENT_ET = EPOLLET;
    constexpr const int EVENT_ALL = EVENT_READ | EVENT_WRITE | EVENT_ET;
#endif

    class Channel
    {
    public:
        uint32_t events{};
        void *data{};
        int _fd{-1};

    public:
        Channel() = default;

        ~Channel() = default;

        Channel(int fd, uint32_t events, void *data) : _fd(fd), events(events), data(data)
        {
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