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


    static int indexOf(const String &buff, const String &key)
    {
        size_t index = buff.find(key);
        return index == std::string::npos ? -1 : static_cast<int>(index);
    }

    static String urlDecode(const String &url)
    {
        String result;
        for (size_t i = 0; i < url.length(); i++)
        {
            if (url[i] == '%')
            {
                result += url[i + 1];
                result += url[i + 2];
                i += 2;
            } else
            {
                result += url[i];
            }
        }
        return result;
    }

    static String urlEncode(const String &url)
    {
        String result;
        for (size_t i = 0; i < url.length(); i++)
        {
            if (url[i] == ' ')
            {
                result += "%20";
            } else
            {
                result += url[i];
            }
        }
        return result;
    }

    static void replace(String &str, const String &oldSubstr, const String &newSubstr, int n)
    {
        size_t pos = 0;
        int i = 0;
        while ((pos = str.find(oldSubstr, pos)) != String::npos && n >0 && i++ < n)
        {
            str.replace(pos, oldSubstr.length(), newSubstr);
            pos += newSubstr.length();
        }
    }

    static void replaceAll(std::string &str, const std::string &oldSubstr, const std::string &newSubstr)
    {
        replace(str, oldSubstr, newSubstr, -1);
    }
}