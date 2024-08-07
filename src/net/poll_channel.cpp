#include <fcntl.h>
#include "net/poll_channel.h"

namespace cpptools::net
{
    void cpptools::net::Channel::enableReading()
    {
        events |= EPOLLIN;
    }

    int Channel::getFd() const
    {
        return this->data.fd;
    }

    void Channel::enableWriting()
    {
        events |= EPOLLOUT;
    }

    void Channel::disableAll()
    {
        events = 0;
    }

    void Channel::enableAll()
    {
        events = EPOLLIN | EPOLLET;
        this->enableNoBlocking();
    }

    void Channel::enableEt()
    {
        events |= EPOLLET;
    }

    void Channel::enableNoBlocking() const
    {
        fcntl(this->data.fd, F_SETFL, O_NONBLOCK);
    }
}

