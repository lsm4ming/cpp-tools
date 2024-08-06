#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <utility>
#include "define.h"
#include "net/server_socket.h"
#include "net/inet_address.h"
#include "channel.h"

using namespace cpptools::common;
using namespace cpptools::net;

namespace cpptools::pollEvent
{
    class Acceptor
    {
    private:
        EventLoop *loop_;               // Acceptor对应的事件循环，在构造函数中传入。
        ServerSocket servsock_;                // 服务端用于监听的socket，在构造函数中创建。
        Channel acceptchannel_;    // Acceptor对应的channel，在构造函数中创建。
        std::function<void(std::unique_ptr<Socket>)> newconnectioncb_;   // 处理新客户端连接请求的回调函数

    public:
        Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);

        ~Acceptor();

        void newconnection();    // 处理新客户端连接请求。

        // 设置处理新客户端连接请求的回调函数，将在创建Acceptor对象的时候（TcpServer类的构造函数中）设置。
        void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn);
    };
}