#include "net/poll_channel.h"

namespace cpptools::net
{
    void cpptools::net::Channel::enableReading()
    {
        events |= EPOLLIN;
    }

    int Channel::getFd() const
    {
        return this->_fd;
    }
}

