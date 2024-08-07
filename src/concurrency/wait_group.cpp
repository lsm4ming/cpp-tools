#include "concurrency/wait_group.h"

namespace cpptools::concurrency
{
    void WaitGroup::add(int n)
    {
        count += n;
        if (count == 0)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condition.notify_all();
        }
    }

    void WaitGroup::done()
    {
        this->add(-1);
    }

    void WaitGroup::wait()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (count > 0)
        {
            _condition.wait(lock);
        }
    }
}
