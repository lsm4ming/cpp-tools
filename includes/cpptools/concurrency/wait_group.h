#pragma once

#include <atomic>
#include <condition_variable>
#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::concurrency
{
    class WaitGroup
    {
    private:
        std::atomic_int count = 0;
        std::mutex _mutex;
        std::condition_variable _condition;

    public:
        WaitGroup() = default;

        ~WaitGroup() = default;

        void add(int n = 1);

        void done();

        void wait();
    };
}