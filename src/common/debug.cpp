#include "cpptools/common/debug.h"

namespace cpptools::common
{
    void Debug::coreDumpEnable()
    {
        struct rlimit rlimit = {0, 0};
        rlimit.rlim_cur = RLIM_INFINITY;
        rlimit.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &rlimit);
    }

    void Debug::setDebug(bool debug)
    {
        getInstance()._debug = debug;
    }

    bool Debug::isDebug()
    {
        return getInstance()._debug;
    }
}


