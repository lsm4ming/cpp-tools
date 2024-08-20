#include "cpptools/net/poll_channel.h"

namespace cpptools::net
{
    void cpptools::net::Channel::enableReading()
    {
        events |= EVENT_READ;
    }

    int Channel::getFd() const
    {
        return this->_fd;
    }

    void Channel::enableWriting()
    {
        events |= EVENT_WRITE;
    }

    void Channel::disableAll()
    {
        events = 0;
    }

    void Channel::enableAll()
    {
        events = EVENT_ALL;
        this->enableNoBlocking();
    }

    void Channel::enableEt()
    {
        events |= EVENT_ET;
    }

    void Channel::enableNoBlocking() const
    {
        int flags = fcntl(this->_fd, F_GETFL, 0);
        if (flags == -1)
        {
            std::cerr << "Failed to get file descriptor flags: " << std::strerror(errno) << std::endl;
            return;
        }

        if (fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            std::cerr << "Failed to set file descriptor to non-blocking: " << std::strerror(errno) << std::endl;
        }
    }

    int Channel::close() const
    {
        cpptools::log::LOG_DEBUG("服务端主动退出,fd=%d", this->_fd);
        if (this->removeChannel() < 0)
        {
            return -1;
        }
        return ::close(this->_fd);
    }

    int Channel::updateChannel() const
    {
#if defined(OS_LINUX)
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        ev.data.ptr = (void *) this;
        return epoll_ctl(this->poll_fd, EPOLL_CTL_MOD, this->_fd, &ev);
#elif defined(OS_MAC)
        struct kevent ev[2];
        int n = 0;
        if (events & EVENT_READ)
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0,
                   (void *) (intptr_t) this->_fd);
        } else
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_READ, EV_DELETE, 0, 0, (void *) (intptr_t) this->_fd);
        }
        if (events & EVENT_WRITE)
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0,
                   (void *) (intptr_t) this->_fd);
        } else
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *) (intptr_t) this->_fd);
        }
        return kevent(this->poll_fd, ev, n, nullptr, 0, nullptr);
#endif
    }

    int Channel::addChannel() const
    {
#if defined(OS_LINUX)
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        cpptools::log::LOG_DEBUG("连接加入,fd=%d", this->_fd);
        return epoll_ctl(this->poll_fd, EPOLL_CTL_ADD, this->_fd, &ev);
#elif defined(OS_MAC)
        struct kevent ev[2];
        int n = 0;
        if (events & EVENT_READ)
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0,
                   (void *) (intptr_t) this->_fd);
        }
        if (events & EVENT_WRITE)
        {
            EV_SET(&ev[n++], this->_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0,
                   (void *) (intptr_t) this->_fd);
        }
        return kevent(this->poll_fd, ev, n, nullptr, 0, nullptr);
#endif
    }

    int Channel::removeChannel() const
    {
#if defined(OS_LINUX)
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        return epoll_ctl(this->poll_fd, EPOLL_CTL_DEL, this->_fd, &ev);
#elif defined(OS_MAC)
        struct kevent ev{};
        EV_SET(&ev, this->_fd, this->events, EV_DELETE, 0, 0, nullptr);
        return kevent(this->poll_fd, &ev, 1, nullptr, 0, nullptr);
#endif
    }


    bool Channel::canWriting() const
    {
        return this->events & EVENT_WRITE;
    }
}

