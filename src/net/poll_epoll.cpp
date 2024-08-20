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
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &event);
    }

    void PollEpoll::close()
    {
        ::close(this->epoll_fd);
        ::close(this->socket_fd);
    }

    int PollEpoll::pollWait(int timeout)
    {
        epoll_event events[MaxEvents];
        int n_fds = epoll_wait(this->epoll_fd, events, MaxEvents, timeout);
        if (n_fds < 0 && EINTR != errno) // 处理 epoll_wait 错误
        {
            cpptools::log::LOG_ERROR("epoll_wait error: %s", strerror(errno));
            return -1;
        }
        for (int i = 0; i < n_fds; i++)
        {
            cpptools::log::LOG_DEBUG("触发事件 events[i].data.fd=%d", events[i].data.fd);
            if (events[i].data.fd == this->socket_fd)  // 连接加入
            {
                Channel channel(this->socket_fd, events[i].events, this->epoll_fd);
                int client_fd;
                while ((client_fd = _handler->onAccept(channel)) > 0)
                {
                    channel._fd = client_fd;
                    channel.enableReading();
                    channel.enableEt();
                    channel.enableNoBlocking();
                    channel.setCloseCallback([this](int fd)
                                             {
                                                 this->channelMap.extract(fd);
                                             });
                    if (channel.addChannel() < 0)
                    {
                        cpptools::log::LOG_ERROR("addChannel error");
                    }
                    this->channelMap[client_fd] = channel;
                }
                continue;
            }
            auto channel = this->channelMap[events[i].data.fd];
            // 连接退出
            if ((events[i].events & (EPOLLERR | EPOLLHUP)))
            {
                cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", events[i].data.fd);
                if (channel.close() < 0)
                {
                    cpptools::log::LOG_ERROR("Failed to remove channel");
                }
                _handler->onClose(channel);
                this->channelMap.extract(events[i].data.fd);
                continue;
            }
            if (events[i].events & EPOLLIN) // 是否可读
            {
                _handler->onRead(channel);
                channel.enableWriting();
            }
            if (events[i].events & EPOLLOUT) // 是否可写
            {
                _handler->onWrite(channel);
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