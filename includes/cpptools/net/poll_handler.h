#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include "poll_channel.h"
#include "inet_address.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class ChannelHandler
    {
    public:
        virtual int onAccept(Channel &channel) = 0;

        virtual ssize_t onRead(const Channel &channel) = 0;

        virtual void onWrite(const Channel &channel) = 0;

        virtual void onClose(const Channel &channel) = 0;

        virtual ~ChannelHandler() = default;
    };

    class PollConn
    {
    private:
        const InetAddress clientAddress;
        const Channel &channel;
        mutable std::ostringstream send_buf{};
        mutable size_t send_len{0};

    public:
        PollConn(InetAddress &addr, const Channel &channel) : clientAddress(addr),
                                                              channel(channel)
        {};

        ~PollConn() = default;

        [[nodiscard]] const InetAddress &getClientAddress() const
        {
            return this->clientAddress;
        }

        ssize_t read(char *buf, size_t len) const;

        size_t write(const void *buf, size_t len) const;

        ssize_t writeConn() const;

        void close() const;

        void flush() const;

        bool finished() const;

        [[nodiscard]] int getFd() const
        {
            return this->channel._fd;
        }
    };

    class ConnectHandler : public ChannelHandler
    {
    public:
        virtual void onAccept(const PollConn &conn) = 0;

        virtual ssize_t onRead(const PollConn &conn) = 0;

        virtual void onWrite(const PollConn &conn) = 0;

        virtual void onClose(const PollConn &conn) = 0;

        int onAccept(Channel &channel) override
        { return 0; };

        ssize_t onRead(const Channel &channel) override
        {
            return 0;
        };

        void onWrite(const Channel &channel) override
        {};

        void onClose(const Channel &channel) override
        {};

        ~ConnectHandler() override = default;
    };

    class HandlerWrapper : public ChannelHandler
    {
    private:
        ConnectHandler *customHandler;
        HashMap<int, SharedPtr<PollConn>> connMap;

    public:
        explicit HandlerWrapper(ConnectHandler *handler);

        explicit HandlerWrapper(ConnectHandler &handler);

        explicit HandlerWrapper(ConnectHandler &&handler);

        int onAccept(Channel &channel) override;

        ssize_t onRead(const Channel &channel) override;

        void onWrite(const Channel &channel) override;

        void onClose(const Channel &channel) override;

        template<typename T>
        static UniquePtr<HandlerWrapper> makeWrapper(T &&handler)
        {
            return std::make_unique<HandlerWrapper>(std::forward<T>(handler));
        }
    };

    extern UniquePtr<PollEvent> createPollEvent(ChannelHandler *handler);

    extern UniquePtr<PollEvent> createPollEvent(UniquePtr<ChannelHandler> handler);
}