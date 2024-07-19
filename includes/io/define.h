#pragma once

#include <fcntl.h>
#include "common/common.h"

namespace cpptools::io
{
    constexpr int OPEN_FLAG = O_RDWR | O_APPEND | O_CLOEXEC | O_NONBLOCK;
    constexpr int READ_FLAG = O_RDONLY | O_CLOEXEC | O_NONBLOCK;
    constexpr int WRITE_FLAG = O_WRONLY | O_CLOEXEC | O_NONBLOCK;
}