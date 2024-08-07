#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include "common/common.h"
#include "thread_pool.h"

using namespace cpptools::common;

namespace cpptools::concurrency
{
    class FixedQuantityThreadPool : protected ThreadPool
    {
    private:
        Vector <Thread> threads_;                          // 线程池中的线程。
        std::queue<Function < void()>> taskqueue_;    // 任务队列。
        std::mutex mutex_;                                                  // 任务队列同步的互斥锁。
        std::condition_variable condition_;                         // 任务队列同步的条件变量。

    public:
        FixedQuantityThreadPool(size_t threadNum, const std::string& name);

        // 把任务添加到队列中。
        void addTask(const Function<void()> &task);

        // 获取线程池的大小。
        [[nodiscard]] size_t size() const;

        // 停止线程。
        void shutdown();

        // 在析构函数中将停止线程。
        ~FixedQuantityThreadPool();
    };
}