#pragma once

#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::net
{
    constexpr const size_t MaxEvents = 1024;

    class PollEvent
    {
    public:
        PollEvent() = default;

        virtual ~PollEvent() = default;

        /**
         * 初始化
         * @return
         */
        virtual int makeup(int fd) = 0;

        /**
         * 关闭
         */
        virtual void close() = 0;

        /**
         * 发起一次poll轮询
         * @param timeout
         * @return
         */
        virtual int pollWait(int timeout) = 0;
    };
}