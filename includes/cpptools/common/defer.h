#pragma once

#include "common.h"

namespace cpptools::common
{
    class Defer
    {
    private:
        std::function<void()> _func;

    public:
        Defer(std::function<void()> func) : _func(func)
        {}

        ~Defer()
        {
            _func();
        };
    };
}