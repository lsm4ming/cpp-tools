#pragma once

#include <sys/epoll.h>
#include "define.h"
#include "net/socket.h"
#include "time/time.h"
#include "buffer.h"
#include "event_loop.h"

using namespace cpptools::common;
using namespace cpptools::net;
using namespace cpptools::time;

namespace cpptools::pollEvent
{
    class Channel
    {
    private:
        int fd_ = -1;                             // Channel拥有的fd，Channel和fd是一对一的关系。
        EventLoop *loop_;                // Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个Channel只对应一个EventLoop。
        bool inepoll_ = false;              // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
        uint32_t events_ = 0;              // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
        uint32_t revents_ = 0;             // fd_已发生的事件。
        std::function<void()> readcallback_;         // fd_读事件的回调函数，如果是acceptchannel，将回调Acceptor::newconnection()，如果是clientchannel，将回调Connection::onmessage()。
        std::function<void()> closecallback_;        // 关闭fd_的回调函数，将回调Connection::closecallback()。
        std::function<void()> errorcallback_;        // fd_发生了错误的回调函数，将回调Connection::errorcallback()。
        std::function<void()> writecallback_;        // fd_写事件的回调函数，将回调Connection::writecallback()。

    public:
        Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd)
        {};

        ~Channel() = default;

        bool inpoll();

        uint32_t events();

        uint32_t revents();

        [[nodiscard]] int getFd() const;

        void setinepoll(bool inepoll);

        void setrevents(uint32_t ev);

        void useet();                                    // 采用边缘触发。
        void enablereading();                     // 让epoll_wait()监视fd_的读事件，注册读事件。
        void disablereading();                    // 取消读事件。
        void enablewriting();                      // 注册写事件。
        void disablewriting();                     // 取消写事件。
        void disableall();                             // 取消全部的事件。
        void remove();                                // 从事件循环中删除Channel。

        void handleevent();         // 事件处理函数，epoll_wait()返回的时候，执行它。

        void setreadcallback(std::function<void()> fn);    // 设置fd_读事件的回调函数。
        void setclosecallback(std::function<void()> fn);   // 设置关闭fd_的回调函数。
        void seterrorcallback(std::function<void()> fn);   // 设置fd_发生了错误的回调函数。
        void setwritecallback(std::function<void()> fn);   // 设置写事件的回调函数。
    };
}