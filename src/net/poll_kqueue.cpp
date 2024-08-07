#include "net/poll_kqueue.h"

#ifdef OS_MAC
namespace cpptools::net
{
    int PollKqueue::makeup(int fd)
    {
        this->socket_fd = fd;
        return (this->kqueue_fd = kqueue());
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
#endif