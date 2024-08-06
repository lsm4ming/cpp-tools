#include "poll_event/poll.h"

namespace cpptools::pollEvent
{
    Poll::Poll()
    {
        if ((this->poll_fd = epoll_create(1)) == -1)
        {
            printf("epoll_create() failed(%d).\n", errno);
            exit(-1);
        }
    }

    Poll::~Poll()
    {
        ::close(this->poll_fd);
    }

    int Poll::updateChannel(Channel *ch) const
    {
        epoll_event ev{};
        ev.data.ptr = ch;
        ev.events = ch->events();
        if (ch->inpoll())
        {
            return epoll_ctl(this->poll_fd, EPOLL_CTL_MOD, ch->getFd(), &ev);
        } else
        {
            if (epoll_ctl(this->poll_fd, EPOLL_CTL_ADD, ch->getFd(), &ev) == -1)
            {
                return -1;
            }
            ch->setinepoll(true);
            return 0;
        }
    }

    int Poll::removeChannel(Channel *ch) const
    {
        if (ch->inpoll())         // 如果channel已经在树上了。
        {
            return epoll_ctl(this->poll_fd, EPOLL_CTL_DEL, ch->getFd(), nullptr);
        }
        return 0;
    }

    Vector<Channel *> Poll::loop(int timeout)
    {
        Vector<Channel *> channels;        // 存放epoll_wait()返回的事件。

        bzero(events_, sizeof(events_));
        int infds = epoll_wait(this->poll_fd, events_, MaxEvents, timeout);       // 等待监视的fd有事件发生。

        // 返回失败。
        if (infds < 0)
        {
            // EBADF ：epfd不是一个有效的描述符。
            // EFAULT ：参数events指向的内存区域不可写。
            // EINVAL ：epfd不是一个epoll文件描述符，或者参数maxevents小于等于0。
            // EINTR ：阻塞过程中被信号中断，epoll_pwait()可以避免，或者错误处理中，解析error后重新调用epoll_wait()。
            // 在Reactor模型中，不建议使用信号，因为信号处理起来很麻烦，没有必要。------ 陈硕
            perror("epoll_wait() failed");
            exit(-1);
        }

        // 超时。
        if (infds == 0)
        {
            // 如果epoll_wait()超时，表示系统很空闲，返回的channels将为空。
            return channels;
        }

        // 如果infds>0，表示有事件发生的fd的数量。
        for (int ii = 0; ii < infds; ii++)       // 遍历epoll返回的数组events_。
        {
            auto *ch = (Channel *) events_[ii].data.ptr;       // 取出已发生事件的channel。
            ch->setrevents(events_[ii].events);                       // 设置channel的revents_成员。
            channels.push_back(ch);
        }

        return channels;
    }
}


