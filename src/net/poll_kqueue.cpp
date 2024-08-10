#include "cpptools/net/poll_kqueue.h"

#ifdef OS_MAC
namespace cpptools::net
{
    int PollKqueue::makeup(int fd)
    {
        this->socket_fd = fd;
        if ((this->kqueue_fd = kqueue()) < 0)
        {
            return -1;
        }
        struct kevent event{};
        EV_SET(&event, this->socket_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, nullptr);
        return kevent(this->kqueue_fd, &event, 1, nullptr, 0, nullptr);
    }

    void PollKqueue::close()
    {
        ::close(this->kqueue_fd);
        ::close(this->socket_fd);
    }

    int PollKqueue::pollWait(int timeout)
    {
        struct kevent events[MaxEvents];
        struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};
        int n_fds = kevent(this->kqueue_fd, nullptr, 0, events, MaxEvents, timeout == -1 ? nullptr : &ts);
        for (int i = 0; i < n_fds; i++)
        {
            Channel channel((int) events[i].ident, events[i].filter, events[i].udata);
            if (events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
            {
                removeChannel(&channel);
                _handler->onClose(channel);
                continue;
            }
            if (events[i].ident == this->socket_fd)  // New connection
            {
                channel._fd = _handler->onAccept(channel);
                channel.enableAll();
                addChannel(&channel);
                continue;
            }
            if (events[i].filter == EVFILT_READ) // Readable
            {
                _handler->onRead(channel);
            }
            if (events[i].filter == EVFILT_WRITE) // Writable
            {
                _handler->onWrite(channel);
            }
        }
        return n_fds;
    }

    int PollKqueue::updateChannel(Channel *ch)
    {
        struct kevent ev{};
        EV_SET(&ev, ch->_fd, ch->events, EV_ADD | EV_CLEAR, 0, 0, nullptr);
        return kevent(this->kqueue_fd, &ev, 1, nullptr, 0, nullptr);
    }

    int PollKqueue::addChannel(Channel *ch)
    {
        struct kevent ev{};
        EV_SET(&ev, ch->_fd, ch->events, EV_ADD | EV_CLEAR, 0, 0, nullptr);
        return kevent(this->kqueue_fd, &ev, 1, nullptr, 0, nullptr);
    }

    int PollKqueue::removeChannel(Channel *ch)
    {
        struct kevent ev{};
        EV_SET(&ev, ch->_fd, ch->events, EV_DELETE, 0, 0, nullptr);
        return kevent(this->kqueue_fd, &ev, 1, nullptr, 0, nullptr);
    }

    UniquePtr<PollEvent> createPollEvent(ChannelHandler *handler)
    {
        return std::make_unique<PollKqueue>(handler);
    }

    UniquePtr<PollEvent> createPollEvent(UniquePtr<ChannelHandler> handler)
    {
        return std::make_unique<PollKqueue>(handler.get());
    }
}
#endif