#pragma once

#include <fcntl.h>
#include <cstring>
#include "cpptools/common/common.h"
#include "cpptools/log/log.h"

#if defined(OS_MAC)
#include <sys/event.h>
#elif defined(OS_LINUX)

#include <sys/epoll.h>

#endif

#include "poll_event.h"

using namespace cpptools::common;

namespace cpptools::net
{
#if defined(OS_MAC)
    constexpr const int EVENT_READ = EVFILT_READ;
    constexpr const int EVENT_WRITE = EVFILT_WRITE;
    constexpr const int EVENT_ET = EV_CLEAR;
    constexpr const int EVENT_ALL = EVENT_READ | EVENT_WRITE | EVENT_ET;
#elif defined(OS_LINUX)
    constexpr const int EVENT_READ = EPOLLIN;
    constexpr const int EVENT_WRITE = EPOLLOUT;
    constexpr const int EVENT_ET = EPOLLET;
    constexpr const int EVENT_ALL = EVENT_READ | EVENT_WRITE | EVENT_ET;
#endif

    class Channel
    {
    public:
        uint32_t events{};
        int _fd{-1};
        int poll_fd{-1};
        std::function<void(int)> closeCallback;

    public:
        Channel() = default;

        ~Channel() = default;

        Channel(int fd) : _fd(fd)
        {}

        Channel(int fd, uint32_t events, int poll_fd) : _fd(fd), events(events), poll_fd(poll_fd)
        {
        };

        void enableReading();

        void enableWriting();

        void disableAll();

        void enableEt();

        void enableAll();

        void enableNoBlocking() const;

        static void enableNoBlocking(int fd);

        [[nodiscard]] int getFd() const;

        [[nodiscard]] int close() const;

        [[nodiscard]] int updateChannel() const;

        [[nodiscard]] int addChannel() const;

        [[nodiscard]] int removeChannel() const;

        void setCloseCallback(const std::function<void(int)> &callback);
    };
}