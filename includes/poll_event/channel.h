#pragma once

#include "define.h"
#include "net/socket.h"
#include "time/time.h"
#include "buffer.h"

using namespace cpptools::common;
using namespace cpptools::net;
using namespace cpptools::time;

namespace cpptools::pollEvent
{
    class Channel
    {
    private:
        EventLoop *loop_;                                           // Connection对应的事件循环，在构造函数中传入。
        std::unique_ptr<Socket> clientsock_;             // 与客户端通讯的Socket。
        std::unique_ptr<Channel> clientchannel_;     // Connection对应的channel，在构造函数中创建。
        Buffer inputbuffer_;             // 接收缓冲区。
        Buffer outputbuffer_;          // 发送缓冲区。
        std::atomic_bool disconnect_;      // 客户端连接是否已断开，如果已断开，则设置为true。

        std::function<void(
                ConnectionPtr)> closecallback_;                   // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
        std::function<void(
                ConnectionPtr)> errorcallback_;                   // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
        std::function<void(ConnectionPtr, std::string & )> onmessagecallback_;   // 处理报文的回调函数，将回调TcpServer::onmessage()。
        std::function<void(
                ConnectionPtr)> sendcompletecallback_;               // 发送数据完成后的回调函数，将回调TcpServer::sendcomplete()。
        Time lastatime_;             // 时间戳，创建Connection对象时为当前时间，每接收到一个报文，把时间戳更新为当前时间。
    };
}