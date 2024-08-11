#pragma once

#include <cstdarg>
#include <cstdio>
#include "cpptools/common/common.h"

using namespace cpptools::common;

namespace cpptools::utils
{
    class Strings
    {
    public:

        /**
         * 分割字符串
         * @param str
         * @param t
         * @return
         */
        static StringList split(const String &str, const String &t);

        /**
         * 查找字符串
         * @param buff
         * @param key
         * @return
         */
        static int indexOf(const String &buff, const String &key);

        /**
         * url解码
         * @param url
         * @return
         */
        static String urlDecode(const String &url);

        /**
         * url编码
         * @param url
         * @return
         */
        static String urlEncode(const String &url);

        /**
         * 转换为小写
         * @param str
         */
        static void toLower(String &str);

        /**
         * 转换为大写
         * @param str
         */
        static void toUpper(String &str);

        /**
         * 格式化字符串
         * @param format
         * @param ...
         * @return
         */
        static String formatString(const char *format, ...);

        /**
         * 格式化字符串
         * @param format
         * @param args
         * @return
         */
        static String formatString(const char *format, va_list args);

        /**
         * 拼接字符串
         * @param list
         * @param sep
         * @return
         */
        static String Join(Vector <String> &list, const String &sep);

        /**
         * 去除字符串前面空格
         * @param str
         * @return
         */
        static String ltrim(const String &str);

        /**
         * 去除字符串后面空格
         * @param str
         * @return
         */
        static String rtrim(const String &str);

        /**
         * 去除字符串前后空格
         * @param str
         * @return
         */
        static String trim(const String &str);
    };
}