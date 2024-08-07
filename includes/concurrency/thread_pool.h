#pragma once

#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::concurrency
{
    #define GET_TID() syscall(SYS_gettid)

    class ThreadPool
    {
    protected:
        // 是否正在运行
        std::atomic_bool running;
        // 线程池名称
        String name;

    public:
        ThreadPool() = default;

        virtual ~ThreadPool() = default;

        /**
         * 加入任务
         * @param task
         */
        virtual void addTask(const Function<void()> &task) = 0;

        /**
         * 获取当前线程池的大小
         * @return
         */
        [[nodiscard]] virtual size_t size() const = 0;

        /**
         * 停止线程池
         */
        virtual void shutdown() = 0;
    };
}