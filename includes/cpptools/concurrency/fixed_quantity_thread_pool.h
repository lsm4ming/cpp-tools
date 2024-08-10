#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "cpptools/common/common.h"
#include "thread_pool.h"

using namespace cpptools::common;

namespace cpptools::concurrency
{
    class FixedQuantityThreadPool : protected ThreadPool
    {
    private:
        Vector<Thread> _threads;
        std::queue<Function<void()>> _taskQueue;
        std::mutex _mutex;
        std::condition_variable _condition;

    public:
        FixedQuantityThreadPool(size_t threadNum, const std::string &name);

        // 把任务添加到队列中。
        void addTask(const Function<void()> &task) override;

        // 获取线程池的大小。
        [[nodiscard]] size_t size() const override;

        // 停止线程。
        void shutdown() override;

        // 在析构函数中将停止线程。
        ~FixedQuantityThreadPool() override;
    };
}