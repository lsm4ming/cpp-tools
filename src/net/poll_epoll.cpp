#include "cpptools/net/poll_epoll.h"

#ifdef OS_LINUX
namespace cpptools::net
{
    PollEpoll::PollEpoll(ChannelHandler *handler)
    {
        this->_handler = handler;
    }

    int PollEpoll::makeup(int fd)
    {
        this->socket_fd = fd;
        if ((this->epoll_fd = epoll_create1(0)) < 0)
        {
            return -1;
        }
        epoll_event event{};
        event.events = EPOLLIN | EPOLLET; // 设置边缘触发
        event.data.fd = this->socket_fd;
        Channel::enableNoBlocking(event.data.fd);
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->socket_fd, &event);
    }

    void PollEpoll::close()
    {
        ::close(this->epoll_fd);
        ::close(this->socket_fd);
    }

    int PollEpoll::pollWait(int timeout)
    {
        epoll_event events[MaxEvents]{};
        int n_fds = epoll_wait(this->epoll_fd, events, MaxEvents, timeout);
        if (n_fds == -1 && errno != EINTR) // 处理 epoll_wait 错误
        {
            cpptools::log::LOG_ERROR("epoll_wait error: %s", strerror(errno));
            return -1;
        }
        for (int i = 0; i < n_fds; i++)
        {
            int fd = events[i].data.fd;
            cpptools::log::LOG_DEBUG("触发事件 events[i].data.fd=%d", fd);
            if (fd == this->socket_fd)  // 连接加入
            {
                auto releaseFunc = [this](int fd)
                {
                    this->channelMap.erase(fd);
                };
                int client_fd;
                do
                {
                    Channel channel(this->socket_fd, events[i].events, this->epoll_fd);
                    client_fd = this->_handler->onAccept(channel);
                    if (client_fd > 0)
                    {
                        channel.enableNoBlocking();
                        channel.setCloseCallback(releaseFunc);
                        channel.events = EPOLLIN | EPOLLET;
                        if (channel.addChannel() < 0)
                        {
                            cpptools::log::LOG_ERROR("addChannel error for fd=%d", client_fd);
                        }
                        this->channelMap[client_fd] = channel;
                    }
                } while (client_fd > 0);
            } else
            {
                int client_fd = events[i].data.fd;
                auto channel = this->channelMap[client_fd];
                // 连接退出
                if ((events[i].events & (EPOLLERR | EPOLLHUP)))
                {
                    cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", client_fd);
                    channel.close();
                    _handler->onClose(channel);
                    continue;
                }
                if (events[i].events & EPOLLIN) // 是否可读
                {
                    ssize_t bytes_read = _handler->onRead(channel);
                    if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        cpptools::log::LOG_INFO("客户端连接异常关闭,fd=%d", client_fd);
                        channel.close();
                    } else if (bytes_read == 0)
                    {
                        cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", client_fd);
                        channel.close();
                        _handler->onClose(channel);
                    } else
                    {
                        channel.events = EPOLLOUT | EPOLLET;
                        if (channel.updateChannel() < 0)
                        {
                            cpptools::log::LOG_ERROR("updateChannel error,reason=%s", strerror(errno));
                        }
                    }
                }
                if (events[i].events & EPOLLOUT) // 是否可写
                {
                    _handler->onWrite(channel);
                }
            }
        }
        return n_fds;
    }

    UniquePtr<PollEvent> createPollEvent(ChannelHandler *handler)
    {
        return std::make_unique<PollEpoll>(handler);
    }

    UniquePtr<PollEvent> createPollEvent(UniquePtr<ChannelHandler> handler)
    {
        return std::make_unique<PollEpoll>(handler.get());
    }
}
#endif