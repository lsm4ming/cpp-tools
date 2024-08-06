#include "net/poll_kqueue.h"

namespace cpptools::net
{
    int PollKqueue::makeup(int fd)
    {
        this->socket_fd = fd;
        if ((this->kqueue_fd = kqueue()) == -1)
        {
            return -1;
        }
        // return updateEvents(this->socket_fd, kReadEvent, false);
        return 0;
    }

    void PollKqueue::close()
    {
    }

    Vector<Channel *> PollKqueue::poll(int timeout)
    {
        return {};
    }

    int PollKqueue::updateChannel(Channel *ch)
    {
        return 0;
    }

    int PollKqueue::addChannel(Channel *ch)
    {
        return 0;
    }

    int PollKqueue::removeChannel(Channel *ch)
    {
        return 0;
    }
}