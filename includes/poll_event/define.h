#pragma once

#include <fcntl.h>
#include <atomic>
#include <functional>
#include "common/common.h"

namespace cpptools::pollEvent
{
    class EventLoop;

    class Poll;

    class Channel;

    class Connection;

    using ConnectionPtr = std::shared_ptr<Connection>;
}