#pragma once

#include <cstring>
#include "define.h"

using namespace cpptools::common;

namespace cpptools::pollEvent
{
    class Buffer
    {
    private:
        std::string buf_;    // 用于存放数据。
        const uint16_t sep_; // 报文的分隔符：0-无分隔符(固定长度、视频会议)；1-四字节的报头；2-"\r\n\r\n"分隔符（http协议）。

    public:
        Buffer(uint16_t sep = 1);

        ~Buffer();

        void append(const char *data, size_t size);

        void appendWithSep(const char *data, size_t size);

        void erase(size_t pos, size_t nn);

        size_t size();

        const char *data();

        void clear();

        bool pickMessage(std::string &ss);
    };
}