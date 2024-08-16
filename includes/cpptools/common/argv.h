#pragma once

#include "common.h"

namespace cpptools::common
{
    class Argv
    {
    private:
        HashMap<String, void *> _argv;

    public:
        Argv() = default;

        Argv(Argv&&) = default;

        Argv &set(const String &name, void *arg)
        {
            _argv[name] = arg;
            return *this;
        }

        template<class T>
        T &get(const String &name)
        {
            auto it = _argv.find(name);
            if (it == _argv.end())
            {
                throw std::runtime_error("arg not found: " + name);
            }
            return *static_cast<T *>(it->second);
        }
    };
}