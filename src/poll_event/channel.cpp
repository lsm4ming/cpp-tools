#include "poll_event/channel.h"

namespace cpptools::pollEvent
{

    bool Channel::inpoll()
    {
        return false;
    }

    uint32_t Channel::events()
    {
        return events_;
    }

    uint32_t Channel::revents()
    {
        return revents_;
    }

    int Channel::getFd() const
    {
        return this->fd_;
    }

    void Channel::setinepoll(bool inepoll)
    {
        inepoll_ = inepoll;
    }

    void Channel::setrevents(uint32_t ev)
    {
        revents_ = ev;
    }

    void Channel::useet()
    {
        events_ = events_ | EPOLLET;
    }

    void Channel::enablereading()
    {
        events_ |= EPOLLIN;
        loop_->updateChannel(this);
    }

    void Channel::disablereading()
    {
        events_ &= ~EPOLLIN;
        loop_->updateChannel(this);
    }

    void Channel::enablewriting()
    {
        events_ |= EPOLLOUT;
        loop_->updateChannel(this);
    }

    void Channel::disablewriting()
    {
        events_ &= ~EPOLLOUT;
        loop_->updateChannel(this);
    }

    void Channel::disableall()
    {
        events_ = 0;
        loop_->updateChannel(this);
    }

    void Channel::remove()
    {
        disableall();                                // 先取消全部的事件。
        loop_->removeChannel(this);    // 从红黑树上删除fd。
    }

    void Channel::handleevent()
    {
        if (revents_ & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
        {
            closecallback_();      // 回调Connection::closecallback()。
        } else if (revents_ & (EPOLLIN | EPOLLPRI))   // 接收缓冲区中有数据可以读。
        {
            readcallback_();   // 如果是acceptchannel，将回调Acceptor::newconnection()，如果是clientchannel，将回调Connection::onmessage()。
        } else if (revents_ & EPOLLOUT)                  // 有数据需要写。
        {
            writecallback_();      // 回调Connection::writecallback()。
        } else                                                           // 其它事件，都视为错误。
        {
            errorcallback_();       // 回调Connection::errorcallback()。
        }
    }

    void Channel::setreadcallback(std::function<void()> fn)
    {
        readcallback_ = fn;
    }

    void Channel::setclosecallback(std::function<void()> fn)
    {
        closecallback_ = fn;
    }

    void Channel::seterrorcallback(std::function<void()> fn)
    {
        errorcallback_ = fn;
    }

    void Channel::setwritecallback(std::function<void()> fn)
    {
        writecallback_ = fn;
    }
}


