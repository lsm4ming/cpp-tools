#include "cpptools/utils/strings.h"

namespace cpptools::utils
{
    StringList Strings::split(const String &str, const String &t)
    {
        StringList ret;
        size_t pos = 0;
        size_t index = str.find(t);
        while (index != String::npos)
        {
            ret.push_back(str.substr(pos, index - pos));
            pos = index + t.size();
            index = str.find(t, pos);
        }
        ret.push_back(str.substr(pos));
        return ret;
    }

    int Strings::indexOf(const String &buff, const String &key)
    {
        size_t index = buff.find(key);
        return index == String::npos ? -1 : static_cast<int>(index);
    }

    String Strings::urlDecode(const String &url)
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

    String Strings::urlEncode(const String &url)
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

    void Strings::toLower(String &str)
    {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
    }

    void Strings::toUpper(String &str)
    {
        transform(str.begin(), str.end(), str.begin(), ::toupper);
    }

    String Strings::formatString(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        String result = formatString(format, args);
        va_end(args);
        return result;
    }

    String Strings::formatString(const char *format, va_list args)
    {
        va_list argsCopy;
        va_copy(argsCopy, args);
        int size = std::vsnprintf(nullptr, 0, format, argsCopy) + 1;
        va_end(argsCopy);
        if (size <= 0)
        {
            throw std::runtime_error("Error during formatting.");
        }
        std::unique_ptr<char[]> buf(new char[size]);
        std::vsnprintf(buf.get(), size, format, args);
        return {buf.get(), buf.get() + size - 1};
    }

    String Strings::Join(Vector<String> &list, const String &sep)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < list.size(); ++i)
        {
            oss << list[i];
            if (i != list.size() - 1)
            {
                oss << sep;
            }
        }
        return oss.str();
    }

    String Strings::trim(const String &str)
    {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, last - first + 1);
    }
}


