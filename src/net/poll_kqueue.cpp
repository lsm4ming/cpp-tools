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
        struct kevent events[MaxEvents]{};
        struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};
        int n_fds = kevent(this->kqueue_fd, nullptr, 0, events, MaxEvents, timeout == -1 ? nullptr : &ts);
        for (int i = 0; i < n_fds; i++)
        {
            auto channel = new Channel((int) events[i].ident, events[i].filter, this->kqueue_fd);
            if (events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
            {
                cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", channel->_fd);
                if (channel->close() < 0)
                {
                    cpptools::log::LOG_ERROR("Failed to remove channel,reason:%s", strerror(errno));
                }
                _handler->onClose(*channel);
                delete channel;
                continue;
            }
            if (events[i].ident == this->socket_fd)  // New connection
            {
                channel->_fd = _handler->onAccept(*channel);
                channel->enableEt();
                channel->enableNoBlocking();
                channel->enableReading();
                if (channel->addChannel() < 0)
                {
                    cpptools::log::LOG_ERROR("addChannel error,reason:%s", strerror(errno));
                }
                continue;
            }
            if (events[i].filter == EVFILT_READ) // Readable
            {
                _handler->onRead(*channel);
                channel->enableWriting();
                std::cout << "监听写事件" << std::endl;
                if (channel->updateChannel() < 0)
                {
                    cpptools::log::LOG_ERROR("updateChannel error,reason:%s", strerror(errno));
                }
                continue;
            }

            if (events[i].filter == EVFILT_WRITE) // Writable
            {
                _handler->onWrite(*channel);
            }
        }
        return n_fds;
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