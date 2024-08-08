#pragma once

#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::utils
{
    static StringList split(const String &str, const String &t)
    {
        StringList ret;
        size_t pos = 0;
        size_t index = str.find(t);
        while (index != std::string::npos)
        {
            ret.push_back(str.substr(pos, index - pos));
            pos = index + t.size();
            index = str.find(t, pos);
        }
        ret.push_back(str.substr(pos));
        return ret;
    }
}