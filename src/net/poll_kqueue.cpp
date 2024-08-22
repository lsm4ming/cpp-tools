#include "cpptools/net/poll_kqueue.h"

#if defined(OS_MAC)
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
        EV_SET(&event, this->socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, nullptr);
        Channel::enableNoBlocking(this->socket_fd);
        return kevent(this->kqueue_fd, &event, 1, nullptr, 0, nullptr) < 0 ? -1 : 0;
    }

    void PollKqueue::close()
    {
        ::close(this->kqueue_fd);
        ::close(this->socket_fd);
    }

    int PollKqueue::pollWait(int timeout)
    {
        struct kevent events[MaxEvents]{};
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
        int n_fds = kevent(this->kqueue_fd, nullptr, 0, events, MaxEvents, (timeout == -1) ? nullptr : &ts);
        if (n_fds == -1 && errno != EINTR) // 处理 kevent 错误
        {
            cpptools::log::LOG_ERROR("kevent error: %s", strerror(errno));
            return -1;
        }

        for (int i = 0; i < n_fds; i++)
        {
            int fd = static_cast<int>(events[i].ident);
            cpptools::log::LOG_DEBUG("触发事件 events[i].ident=%d", fd);

            if (fd == this->socket_fd)
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
            } else
            {
                auto it = this->channelMap.find(fd);
                if (it == this->channelMap.end())
                {
                    cpptools::log::LOG_WARNING("Unknown client_fd=%d", fd);
                    continue;
                }

                Channel &channel = it->second;

                // 处理错误和关闭事件
                if (events[i].flags & (EV_EOF | EV_ERROR))
                {
                    cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", fd);
                    channel.close();
                    _handler->onClose(channel);
                    this->channelMap.erase(fd); // 移除通道
                    continue;
                }

                if (events[i].filter == EVFILT_READ) // Readable
                {
                    ssize_t bytes_read = _handler->onRead(channel);
                    if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        cpptools::log::LOG_INFO("客户端连接异常关闭,fd=%d,reason=%s", fd, strerror(errno));
                        channel.close();
                        this->channelMap.erase(fd); // 移除通道
                    } else if (bytes_read == 0)
                    {
                        cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", fd);
                        channel.close();
                        _handler->onClose(channel);
                        this->channelMap.erase(fd); // 移除通道
                    } else
                    {
                        channel.events = EVFILT_WRITE | EV_CLEAR;
                        if (channel.updateChannel() < 0)
                        {
                            cpptools::log::LOG_ERROR("updateChannel error,reason=%s", strerror(errno));
                        }
                    }
                } else if (events[i].filter == EVFILT_WRITE) // Writable
                {
                    _handler->onWrite(channel);
                }
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
