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
        if (::close(this->_fd) < 0)
        {
            return -1;
        }
        cpptools::log::LOG_DEBUG("服务端主动退出,fd=%d", this->_fd);
        return this->removeChannel();
    }

    int Channel::updateChannel() const
    {
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        ev.data.ptr = (void *) this;
        return epoll_ctl(this->poll_fd, EPOLL_CTL_MOD, this->_fd, &ev);
    }

    int Channel::addChannel() const
    {
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        cpptools::log::LOG_DEBUG("连接加入,fd=%d", this->_fd);
        return epoll_ctl(this->poll_fd, EPOLL_CTL_ADD, this->_fd, &ev);
    }

    int Channel::removeChannel() const
    {
        struct epoll_event ev{};
        ev.events = this->events;
        ev.data.fd = this->_fd;
        return epoll_ctl(this->poll_fd, EPOLL_CTL_DEL, this->_fd, &ev);
    }

    int Channel::write(const String &data)
    {
        this->pendingData.write(data.data(), (long) data.length());
        return (int) data.length();
    }
}

