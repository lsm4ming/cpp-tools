#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <list>
#include <string>

namespace cpptools::reactor
{
    class Packet
    {
    public:
        Packet() = default;

        ~Packet()
        {
            if (data_) delete[] data_;
            len_ = 0;
        }

        uint8_t *Data()
        { return data_; }

        ssize_t Len()
        { return len_; }

    public:
        uint8_t *data_{nullptr};  // 二进制缓冲区
        ssize_t len_{0};          // 缓冲区的长度
    };

    class Codec
    {
    public:
        ~Codec()
        {
            if (msg_) delete msg_;
        }

        void DeCode(uint8_t *data, size_t len)
        {
            if (nullptr == msg_) msg_ = new std::string("");
            msg_->append((const char *) *data, len);
        }

        bool GetMessage(std::string &msg)
        {
            if (nullptr == msg_) return false;
            msg = *msg_;
            delete msg_;
            msg_ = nullptr;
            return true;
        }

    private:
        uint32_t body_len_{0};              // 当前消息的协议体长度
        std::string *msg_{nullptr};         // 解析的消息
    };
}  // namespace EchoServer