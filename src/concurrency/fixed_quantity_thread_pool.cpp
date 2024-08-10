#include "cpptools/concurrency/fixed_quantity_thread_pool.h"

namespace cpptools::concurrency
{
#define SHUTDOWN() if (!running) return; \
    running = false;                         \
    _condition.notify_all();                 \
    for (std::thread &th: _threads) th.join();\


    FixedQuantityThreadPool::FixedQuantityThreadPool(size_t threadNum, const String &name)
    {
        this->running = true;
        this->name = name;
        for (size_t ii = 0; ii < threadNum; ii++)
        {
            _threads.emplace_back([this]
                                  {
                                      // 显示线程类型和线程ID
                                      std::cout << "create " << this->name << " thread(" << std::this_thread::get_id() << ")." << std::endl;
                                      while (running)
                                      {
                                          std::function<void()> task;       // 用于存放出队的元素。
                                          {
                                              std::unique_lock<std::mutex> lock(this->_mutex);
                                              // 等待生产者的条件变量。
                                              this->_condition.wait(lock, [this]
                                              {
                                                  return (!this->running || !this->_taskQueue.empty());
                                              });

                                              // 在线程池停止之前，如果队列中还有任务，执行完再退出。
                                              if (!this->running && this->_taskQueue.empty()) return;
                                              // 出队一个任务。
                                              task = std::move(this->_taskQueue.front());
                                              this->_taskQueue.pop();
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
            std::lock_guard<std::mutex> lock(_mutex);
            _taskQueue.push(task);
        }
        _condition.notify_one();   // 唤醒一个线程。
    }

    size_t FixedQuantityThreadPool::size() const
    {
        return this->_threads.size();
    }

    void FixedQuantityThreadPool::shutdown()
    {
        SHUTDOWN()
    }
}