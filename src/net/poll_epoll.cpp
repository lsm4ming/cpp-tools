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
        if ((this->epoll_fd = epoll_create(1)) < 0)
        {
            return -1;
        }
        epoll_event event{};
        event.events = EPOLLIN | EPOLLET; // 设置边缘触发
        event.data.fd = this->socket_fd;
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
            Channel channel(events[i].data.fd, events[i].events, this->epoll_fd);
            cpptools::log::LOG_DEBUG("触发事件 events[i].data.fd=%d", events[i].data.fd);
            // 连接退出
            if ((events[i].events & (EPOLLERR | EPOLLHUP)))
            {
                cpptools::log::LOG_DEBUG("客户端连接关闭,fd=%d", events[i].data.fd);
                if (channel.removeChannel() < 0)
                {
                    cpptools::log::LOG_ERROR("Failed to remove channel");
                }
                // 关闭fd
                ::close(events[i].data.fd);
                _handler->onClose(channel);
                continue;
            }
            if (events[i].data.fd == this->socket_fd)  // 连接加入
            {
                channel._fd = _handler->onAccept(channel);
                channel.enableAll();
                if (channel.addChannel() < 0)
                {
                    cpptools::log::LOG_ERROR("addChannel error");
                }
                continue;
            }
            if (events[i].events & EPOLLIN) // 是否可读
            {
                _handler->onRead(channel);
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