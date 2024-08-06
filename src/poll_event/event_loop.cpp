#include "poll_event/event_loop.h"

namespace cpptools::pollEvent
{
    int createtimerfd(int sec = 30)
    {
        int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);   // 创建timerfd。
        struct itimerspec timeout{};                                // 定时时间的数据结构。
        memset(&timeout, 0, sizeof(struct itimerspec));
        timeout.it_value.tv_sec = sec;                             // 定时时间，固定为5，方便测试。
        timeout.it_value.tv_nsec = 0;
        timerfd_settime(tfd, 0, &timeout, nullptr);
        return tfd;
    }

// 在构造函数中创建Epoll对象ep_。
    EventLoop::EventLoop(bool mainloop, int timetvl, int timeout) : ep_(new Poll), mainloop_(mainloop),
                                                                    timetvl_(timetvl), timeout_(timeout), stop_(false),
                                                                    wakeupfd_(eventfd(0, EFD_NONBLOCK)),
                                                                    wakechannel_(new Channel(this, wakeupfd_)),
                                                                    timerfd_(createtimerfd(timeout_)),
                                                                    timerchannel_(new Channel(this, timerfd_))
    {
        wakechannel_->setreadcallback([this]
                                      { handlewakeup(); });
        wakechannel_->enablereading();
        timerchannel_->setreadcallback([this]
                                       { handletimer(); });
        timerchannel_->enablereading();
    }

    void EventLoop::run()
    {
        threadid_ = syscall(SYS_gettid);    // 获取事件循环所在线程的id。

        while (!stop_)        // 事件循环。
        {
            std::vector<Channel *> channels = ep_->loop(10 * 1000);         // 等待监视的fd有事件发生。

            // 如果channels为空，表示超时，回调TcpServer::epolltimeout()。
            if (channels.empty())
            {
                epolltimeoutcallback_(this);
            } else
            {
                for (auto &ch: channels)
                {
                    ch->handleevent();        // 处理epoll_wait()返回的事件。
                }
            }
        }
    }

    void EventLoop::stop()
    {
        stop_ = true;
        wakeup();       // 唤醒事件循环，如果没有这行代码，事件循环将在下次闹钟响时或epoll_wait()超时时才会停下来。
    }

    void EventLoop::updateChannel(Channel *ch)
    {
        ep_->updateChannel(ch);
    }

    void EventLoop::removeChannel(Channel *ch)
    {
        ep_->removeChannel(ch);
    }

    void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop *)> fn)
    {
        epolltimeoutcallback_ = fn;
    }

    bool EventLoop::isinloopthread()
    {
        return threadid_ == syscall(SYS_gettid);
    }

    void EventLoop::queueinloop(std::function<void()> fn)
    {
        {
            std::lock_guard<std::mutex> gd(mutex_);           // 给任务队列加锁。
            taskqueue_.push(fn);                                            // 任务入队。
        }
        wakeup();        // 唤醒事件循环。
    }

    void EventLoop::wakeup()
    {
        uint64_t val = 1;
        write(wakeupfd_, &val, sizeof(val));
    }

    void EventLoop::handlewakeup()
    {
        uint64_t val;
        read(wakeupfd_, &val, sizeof(val));       // 从eventfd中读取出数据，如果不读取，eventfd的读事件会一直触发。

        std::function<void()> fn;

        std::lock_guard<std::mutex> gd(mutex_);           // 给任务队列加锁。
        // 执行队列中全部的发送任务。
        while (!taskqueue_.empty())
        {
            fn = std::move(taskqueue_.front());    // 出队一个元素。
            taskqueue_.pop();
            fn();                                                    // 执行任务。
        }
    }

    void EventLoop::handletimer()
    {
        // 重新计时。
        struct itimerspec timeout{};                                // 定时时间的数据结构。
        memset(&timeout, 0, sizeof(struct itimerspec));
        timeout.it_value.tv_sec = timetvl_;                             // 定时时间，固定为5，方便测试。
        timeout.it_value.tv_nsec = 0;
        timerfd_settime(timerfd_, 0, &timeout, 0);

        if (mainloop_)
        {
            // printf("主事件循环的闹钟时间到了。\n");
        } else
        {
            // printf("从事件循环的闹钟时间到了。\n");
            // 获取当前时间
            auto now = Time::now();
            for (auto aa: conns_)
            {
                if (aa.second->timeout(now, timeout_))
                {
                    {
                        std::lock_guard<std::mutex> gd(mmutex_);
                        conns_.erase(aa.first);               // 从EventLoop的map中删除超时的conn。
                    }
                    timercallback_(aa.first);             // 从TcpServer的map中删除超时的conn。
                }
            }
        }
    }

    void EventLoop::newconnection(ConnectionPtr conn)
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[conn->fd()] = conn;
    }

    void EventLoop::settimercallback(std::function<void(int)> fn)
    {
        timercallback_ = std::move(fn);
    }
}

