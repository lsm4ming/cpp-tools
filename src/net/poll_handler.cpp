#include "cpptools/net/poll_handler.h"

namespace cpptools::net
{
    HandlerWrapper::HandlerWrapper(ConnectHandler *handler)
    {
        this->customHandler = handler;
    }

    int HandlerWrapper::onAccept(Channel &channel)
    {
        struct sockaddr_in client_addr{};
        socklen_t client{};
        int client_fd = accept(channel.getFd(), (struct sockaddr *) &client_addr, &client);
        if (client_fd < 0)
        {
            return client_fd;
        }
        channel._fd = client_fd;
        InetAddress address(client_addr);
        auto conn = std::make_shared<PollConn>(address, channel);
        this->connMap.insert({client_fd, conn});
        this->customHandler->onAccept(*conn);
        return client_fd;
    }

    ssize_t HandlerWrapper::onRead(const Channel &channel)
    {
        auto iter = connMap.find(channel.getFd());
        if (iter != connMap.end())
        {
            return this->customHandler->onRead(*iter->second);
        }
        return 0;
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
        return ::read(this->channel._fd, buf, len);
    }

    size_t PollConn::write(const void *buf, size_t len) const
    {
        this->send_buf.write(static_cast<const char *>(buf), (long) len);
        return len;
    }

    ssize_t PollConn::writeConn() const
    {
        ssize_t ret = ::write(this->channel._fd, this->send_buf.str().data() + this->send_len,
                              this->send_buf.str().length() - this->send_len);
        if (ret > 0)
        {
            this->send_len += ret;
        }
        return ret;
    }

    void PollConn::close() const
    {
        channel.close();
    }

    void PollConn::flush() const
    {
        ::fsync(this->channel._fd);
    }

    bool PollConn::finished() const
    {
        return this->send_len == this->send_buf.str().length();
    }
}