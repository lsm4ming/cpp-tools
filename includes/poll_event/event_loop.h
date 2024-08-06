#pragma once

#include <functional>
#include <unistd.h>
#include <mutex>
#include <queue>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include "channel.h"
#include "define.h"
#include "poll.h"
#include "connection.h"
#include "time/time.h"

using namespace cpptools::common;
using namespace cpptools::time;

namespace cpptools::pollEvent
{
    class EventLoop
    {
    private:
        int timetvl_;                                                             // 闹钟时间间隔，单位：秒。。
        int timeout_;                                                           // Connection对象超时的时间，单位：秒。
        std::unique_ptr<Poll> ep_;                                   // 每个事件循环只有一个Epoll。
        std::function<void(EventLoop *)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
        pid_t threadid_{};                                                       // 事件循环所在线程的id。
        std::queue<std::function<void()>> taskqueue_;    // 事件循环线程被eventfd唤醒后执行的任务队列。
        std::mutex mutex_;                                                  // 任务队列同步的互斥锁。
        int wakeupfd_;                                                         // 用于唤醒事件循环线程的eventfd。
        std::unique_ptr<Channel> wakechannel_;              // eventfd的Channel。
        int timerfd_;                                                             // 定时器的fd。
        std::unique_ptr<Channel> timerchannel_;              // 定时器的Channel。
        bool mainloop_;                                                       // true-是主事件循环，false-是从事件循环。
        std::mutex mmutex_;                                              // 保护conns_的互斥锁。
        std::map<int, ConnectionPtr> conns_;                      // 存放运行在该事件循环上全部的Connection对象。
        std::function<void(int)> timercallback_;
        std::atomic_bool stop_;                                            // 初始值为false，如果设置为true，表示停止事件循环。

    public:
        explicit EventLoop(bool mainloop, int timetvl = 10, int timeout = 20);                   // 在构造函数中创建Epoll对象ep_。
        ~EventLoop() = default;

        void run();                      // 运行事件循环。
        void stop();                    // 停止事件循环。

        void updateChannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
        void removeChannel(Channel *ch);                       // 从黑树上删除channel。
        void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);  // 设置epoll_wait()超时的回调函数。

        bool isinloopthread();   // 判断当前线程是否为事件循环线程。

        void queueinloop(std::function<void()> fn);          // 把任务添加到队列中。
        void wakeup();                                                        // 用eventfd唤醒事件循环线程。
        void handlewakeup();                                             // 事件循环线程被eventfd唤醒后执行的函数。

        void handletimer();                                                 // 闹钟响时执行的函数。

        void newconnection(ConnectionPtr conn);            // 把Connection对象保存在conns_中。

        void settimercallback(std::function<void(int)> fn);  // 将被设置为TcpServer::removeconn()
    };
}

