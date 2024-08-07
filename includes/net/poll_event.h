#pragma once

#include "common/common.h"
#include "poll_handler.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class Channel;

    class ChannelHandler;

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

        /**
         * 更新Channel
         * @param ch
         * @return
         */
        virtual int updateChannel(Channel *ch) = 0;

        /**
         * 添加Channel
         * @param ch
         * @return
         */
        virtual int addChannel(Channel *ch) = 0;

        /**
         * 删除Channel
         * @param ch
         * @return
         */
        virtual int removeChannel(Channel *ch) = 0;
    };

    extern UniquePtr<PollEvent> createPollEvent(ChannelHandler *handler);

    extern UniquePtr<PollEvent> createPollEvent(UniquePtr<ChannelHandler> handler);
}