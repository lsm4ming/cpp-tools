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
        fcntl(this->_fd, F_SETFL, O_NONBLOCK);
    }
}

