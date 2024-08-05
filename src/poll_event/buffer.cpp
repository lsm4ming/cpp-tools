#include "poll_event/buffer.h"

namespace cpptools::pollEvent
{
    Buffer::Buffer(uint16_t sep) : sep_(sep)
    {

    }

    Buffer::~Buffer() = default;

    void Buffer::append(const char *data, size_t size)
    {
        buf_.append(data, size);
    }

    void Buffer::appendWithSep(const char *data, size_t size)
    {
        if (sep_ == 0)             // 没有分隔符。
        {
            buf_.append(data, size);                    // 处理报文内容。
        } else if (sep_ == 1)     // 四字节的报头。
        {
            buf_.append((char *) &size, 4);           // 处理报文长度（头部）。
            buf_.append(data, size);                    // 处理报文内容。
        }
    }

    void Buffer::erase(size_t pos, size_t nn)
    {
        buf_.erase(pos, nn);
    }

    size_t Buffer::size()
    {
        return buf_.size();
    }

    const char *Buffer::data()
    {
        return buf_.data();
    }

    void Buffer::clear()
    {
        buf_.clear();
    }

    bool Buffer::pickMessage(std::string &ss)
    {
        if (buf_.empty()) return false;
        if (sep_ == 0)
        {
            ss = buf_;
            buf_.clear();
        } else if (sep_ == 1)
        {
            int len;
            memcpy(&len, buf_.data(), 4);
            if (buf_.size() < len + 4) return false;
            ss = buf_.substr(4, len);
            buf_.erase(0, len + 4);
        }
        return true;
    }
}