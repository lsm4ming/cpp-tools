#include "net/poll_epoll.h"
#include "net/poll_channel.h"

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
        event.events = EPOLLIN;
        event.data.fd = this->socket_fd;
        event.events |= EPOLLET; // 设置边缘触发
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
        for (int i = 0; i < n_fds; i++)
        {
            auto *ch = reinterpret_cast<Channel *>(events[i].data.ptr);
            // 连接退出
            if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR) || (!(events[i].events & EPOLLIN)))
            {
                removeChannel(ch);
                _handler->onClose(*ch);
                continue;
            }
            if (events[i].data.fd == this->socket_fd)  // 连接加入
            {
                addChannel(ch);
                _handler->onAccept(*ch);
                continue;
            }
            if (events[i].events & EPOLLIN) // 是否可读
            {
                _handler->onRead(*ch);
            }
            if (events[i].events & EPOLLOUT) // 是否可写
            {
                _handler->onWrite(*ch);
            }
        }
        return n_fds;
    }

    int PollEpoll::updateChannel(Channel *ch)
    {
        epoll_event ev{};
        ev.events = ch->events;
        ev.data.ptr = ch;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, this->socket_fd, &ev);
    }

    int PollEpoll::addChannel(Channel *ch)
    {
        epoll_event ev{};
        ev.events = ch->events;
        ev.data.ptr = ch;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->socket_fd, &ev);
    }

    int PollEpoll::removeChannel(Channel *ch)
    {
        epoll_event ev{};
        ev.events = ch->events;
        ev.data.ptr = ch;
        return epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, this->socket_fd, &ev);
    }

    UniquePtr<PollEvent> createPollEvent(ChannelHandler *handler)
    {
        return std::make_unique<PollEpoll>(handler);
    }

    UniquePtr<PollEvent> createPollEvent(UniquePtr<ChannelHandler> handler)
    {
        return std::make_unique<PollEpoll>(handler.release());
    }
}
#endif