#include "concurrency/fixed_quantity_thread_pool.h"

namespace cpptools::concurrency
{
#define SHUTDOWN() if (!running) return; \
    running = false;                         \
    condition_.notify_all();                 \
    for (std::thread &th: threads_) th.join();\


    FixedQuantityThreadPool::FixedQuantityThreadPool(size_t threadNum, const String &name)
    {
        this->running = true;
        this->name = name;
        for (size_t ii = 0; ii < threadNum; ii++)
        {
            threads_.emplace_back([this]
                                  {
                                      printf("create %s thread(%ld).\n", this->name.c_str(),
                                             GET_TID());     // 显示线程类型和线程ID。
                                      while (running)
                                      {
                                          std::function<void()> task;       // 用于存放出队的元素。
                                          {
                                              std::unique_lock<std::mutex> lock(this->mutex_);
                                              // 等待生产者的条件变量。
                                              this->condition_.wait(lock, [this]
                                              {
                                                  return (!this->running || !this->taskqueue_.empty());
                                              });

                                              // 在线程池停止之前，如果队列中还有任务，执行完再退出。
                                              if (!this->running && this->taskqueue_.empty()) return;
                                              // 出队一个任务。
                                              task = std::move(this->taskqueue_.front());
                                              this->taskqueue_.pop();
                                          }
                                          task();  // 执行任务。
                                      }
                                  });
        }
    }

    FixedQuantityThreadPool::~FixedQuantityThreadPool()
    {
        SHUTDOWN()
    }

    void FixedQuantityThreadPool::addTask(const Function<void()> &task)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            taskqueue_.push(task);
        }
        condition_.notify_one();   // 唤醒一个线程。
    }

    size_t FixedQuantityThreadPool::size() const
    {
        return this->threads_.size();
    }

    void FixedQuantityThreadPool::shutdown()
    {
        SHUTDOWN()
    }
}