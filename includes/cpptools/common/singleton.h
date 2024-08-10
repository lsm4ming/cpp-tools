#pragma once

#include "common.h"

namespace cpptools::common
{
    template <class T>
    class Singleton
    {
    public:
        static T *getInstance()
        {
            static T instance;
            return &instance;
        }
    };
}