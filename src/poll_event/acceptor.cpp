#include "poll_event/acceptor.h"

namespace cpptools::pollEvent
{
    Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port)
            : loop_(loop), servsock_(ip, port), acceptchannel_(loop_, servsock_.getFd())
    {
        servsock_.setBlocking(true);
        servsock_.setReuseaddr(true);
        servsock_.setTcpNoDelay(true);
        servsock_.setReuseport(true);
        servsock_.setKeepalive(true);
        servsock_.bind();
        servsock_.listen();
        acceptchannel_.setreadcallback([this]
                                       { newconnection(); });
        acceptchannel_.enablereading(); // 让epoll_wait()监视servchannel的读事件。
    }

    Acceptor::~Acceptor() = default;

// 处理新客户端连接请求。
    void Acceptor::newconnection()
    {
        InetAddress clientaddr;             // 客户端的地址和协议。
        std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
        clientsock->setHost(clientaddr.ip());
        clientsock->setPort(clientaddr.port());
        newconnectioncb_(std::move(clientsock));      // 回调TcpServer::newconnection()。
    }

    void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn)       // 设置处理新客户端连接请求的回调函数。
    {
        newconnectioncb_ = std::move(fn);
    }
}