#include "cpptools/net/poll_handler.h"

namespace cpptools::net
{
    HandlerWrapper::HandlerWrapper(ConnectHandler *handler)
    {
        this->customHandler = handler;
    }

    int HandlerWrapper::onAccept(const Channel &channel)
    {
        struct sockaddr_in client_addr{};
        socklen_t client{};
        int client_fd = accept(channel.getFd(), (struct sockaddr *) &client_addr, &client);
        if (client_fd == -1 && errno != EINTR)
        {
            perror("accept error");
            throw std::runtime_error("accept error");
        }
        InetAddress address(client_addr);
        auto conn = std::make_shared<PollConn>(client_fd, address, channel);
        this->connMap.insert({client_fd, conn});
        this->customHandler->onAccept(*conn);
        return client_fd;
    }

    void HandlerWrapper::onRead(const Channel &channel)
    {
        auto iter = connMap.find(channel.getFd());
        if (iter != connMap.end())
        {
            this->customHandler->onRead(*iter->second);
        }
    }

    void HandlerWrapper::onWrite(const Channel &channel)
    {
        auto iter = connMap.find(channel.getFd());
        if (iter != connMap.end())
        {
            this->customHandler->onWrite(*iter->second);
        }
    }

    void HandlerWrapper::onClose(const Channel &channel)
    {
        auto iter = connMap.find(channel.getFd());
        if (iter != connMap.end())
        {
            this->customHandler->onClose(*iter->second);
            connMap.extract(channel.getFd());
        }
    }

    HandlerWrapper::HandlerWrapper(ConnectHandler &handler)
    {
        this->customHandler = &handler;
    }

    HandlerWrapper::HandlerWrapper(ConnectHandler &&handler)
    {
        this->customHandler = &handler;
    }

    ssize_t PollConn::read(char *buf, size_t len) const
    {
        return ::read(this->_fd, buf, len);
    }

    size_t PollConn::write(const void *buf, size_t len) const
    {
        this->send_buf.write(static_cast<const char *>(buf), (long) len);
        return len;
    }

    ssize_t PollConn::writeConn() const
    {
        ssize_t ret = ::write(this->_fd, this->send_buf.str().data() + this->send_len,
                              this->send_buf.str().length() - this->send_len);
        if (ret > 0)
        {
            this->send_len += ret;
        }
        return ret;
    }

    int PollConn::close() const
    {
        std::cout << "连接关闭" << std::endl;
        return this->channel.close();
    }

    void PollConn::flush() const
    {
        ::fsync(this->_fd);
    }

    bool PollConn::finished() const
    {
        return this->send_len == this->send_buf.str().length();
    }
}