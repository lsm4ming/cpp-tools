#pragma once

#include "common.hpp"

namespace cpptools::reactor
{
    // http响应数据
    class Conn
    {
    public:
        Conn(int fd, int epoll_fd, bool is_multi_io) : fd_(fd), epoll_fd_(epoll_fd), is_multi_io_(is_multi_io)
        {}

        [[nodiscard]] bool Read() const
        {
            do
            {
                uint8_t data[100];
                ssize_t ret = read(fd_, data, 100);  // 一次最多读取100字节
                if (ret == 0)
                {
                    perror("peer close connection");
                    return false;
                }
                if (ret < 0)
                {
                    if (EINTR == errno) continue;
                    if (EAGAIN == errno or EWOULDBLOCK == errno) return true;
                    perror("read failed");
                    return false;
                }
            } while (is_multi_io_);
            return true;
        }

        bool Write(bool autoEnCode = true)
        {
            do
            {
                if (send_len_ == this->message_.length()) return true;
                ssize_t ret = write(fd_, this->message_.data() + send_len_, this->message_.length() - send_len_);
                if (ret < 0)
                {
                    if (EINTR == errno) continue;
                    if (EAGAIN == errno && EWOULDBLOCK == errno) return true;
                    perror("write failed");
                    return false;
                }
                send_len_ += ret;
            } while (is_multi_io_);
            return true;
        }

        bool OneMessage()
        {
            this->message_.clear();
            this->message_ = "HTTP/1.1 200 OK" + std::string("\r\n") +
                             "Content-Type: application/json; charset=utf-8" + std::string("\r\n") +
                             "Content-Length: 51" + std::string("\r\n\r\n");
            this->message_ += R"({"code":200,"data":"pong","message":"操作成功"})";
            return true;
        }

        bool FinishWrite()
        { return send_len_ == this->message_.length(); }

        [[nodiscard]] int Fd() const
        { return fd_; }

        [[nodiscard]] int EpollFd() const
        { return epoll_fd_; }

    private:
        int fd_{0};            // 关联的客户端连接fd
        int epoll_fd_{0};      // 关联的epoll实例的fd
        bool is_multi_io_;     // 是否做多次io，直到返回EAGAIN或者EWOULDBLOCK
        ssize_t send_len_{0};  // 要发送的应答数据的长度
        std::string message_;  // 对于EchoServer来说，即是获取的请求消息，也是要发送的应答消息
    };
}  // namespace EchoServer