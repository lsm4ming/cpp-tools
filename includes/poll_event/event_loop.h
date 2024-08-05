#pragma once

#include <functional>
#include <queue>
#include "channel.h"
#include "define.h"

using namespace cpptools::common;

namespace cpptools::pollEvent
{
    class EventLoop
    {
    private:
    private:
        int timetvl_;                                                             // 闹钟时间间隔，单位：秒。。
        int timeout_;                                                           // Connection对象超时的时间，单位：秒。
        std::unique_ptr<Poll> ep_;                                   // 每个事件循环只有一个Epoll。
        std::function<void(EventLoop *)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
        pid_t threadid_;                                                       // 事件循环所在线程的id。
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
        explicit EventLoop();
    };
}

