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
        struct timespec ts{};
        if (timeout == -1)
        {
            ts.tv_sec = 0;
            ts.tv_nsec = 0;
        } else
        {
            ts.tv_sec = timeout / 1000;
            ts.tv_nsec = (timeout % 1000) * 1000000;
        }
        int n_fds = kevent(this->kqueue_fd, nullptr, 0, events, MaxEvents, &ts);
        for (int i = 0; i < n_fds; i++)
        {
            if (events[i].ident == this->socket_fd)
            {
                auto releaseFunc = [this](int fd)
                {
                    this->channelMap.erase(fd);
                };
                int client_fd;
                do
                {
                    Channel channel(this->socket_fd, events[i].flags, this->kqueue_fd);
                    client_fd = this->_handler->onAccept(channel);
                    if (client_fd > 0)
                    {
                        channel.enableNoBlocking();
                        channel.setCloseCallback(releaseFunc);
                        channel.events = EVFILT_READ | EV_CLEAR;
                        if (channel.addChannel() < 0)
                        {
                            cpptools::log::LOG_ERROR("addChannel error for fd=%d", client_fd);
                        }
                        this->channelMap[client_fd] = channel;
                    }
                } while (client_fd > 0);
                continue;
            }

            int client_fd = static_cast<int>(events[i].ident);
            auto channel = this->channelMap[client_fd];

            if (events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
            {
                cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", channel._fd);
                channel.close();
                _handler->onClose(channel);
                continue;
            }
            if (events[i].filter == EVFILT_READ) // Readable
            {
                ssize_t bytes_read = _handler->onRead(channel);
                if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    cpptools::log::LOG_INFO("客户端连接异常关闭,fd=%d,reason=%s", client_fd, strerror(errno));
                    channel.close();
                    continue;
                } else if (bytes_read == 0)
                {
                    cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", client_fd);
                    channel.close();
                    _handler->onClose(channel);
                } else
                {
                    channel.events = EVFILT_WRITE | EV_CLEAR;
                    if (channel.updateChannel() < 0)
                    {
                        cpptools::log::LOG_ERROR("updateChannel error,reason=%s", strerror(errno));
                    }
                }
            }
            if (events[i].filter == EVFILT_WRITE) // Writable
            {
                _handler->onWrite(channel);
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