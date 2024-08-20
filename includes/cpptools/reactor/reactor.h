#pragma once

#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

#include "cpptools/common/system.h"
#include "cpptools/common/argv.h"
#include "cpptools/reactor/epollctl.hpp"

using namespace cpptools::common;

namespace cpptools::reactor
{
    int *EpollFd;
    int EpollInitCnt = 0;
    std::mutex Mutex;
    std::condition_variable Cond;

    void waitSubReactor()
    {
        std::unique_lock<std::mutex> locker(Mutex);
        Cond.wait(locker, []() -> bool
        { return EpollInitCnt >= cpptools::common::System::cpuNumber(); });
    }

    void subReactorNotifyReady()
    {
        {
            std::unique_lock<std::mutex> locker(Mutex);
            EpollInitCnt++;
        }
        Cond.notify_all();
    }

    void addToSubReactor(int &index, int clientFd)
    {
        index++;
        index %= (int) System::cpuNumber();
        // 轮询的方式添加到SubReactor线程中
        auto *conn = new Conn(clientFd, EpollFd[index], true);
        AddReadEvent(conn);  // 监听可读事件
    }

    void MainReactor(std::array<String, 3> argv)
    {
        waitSubReactor();  // 等待所有的SubReactor线程都启动完毕
        int sockFd = CreateListenSocket(argv[0], std::stol(argv[1]), true);
        if (sockFd < 0)
        {
            return;
        }
        epoll_event events[2048];
        int epollFd = epoll_create(1024);
        if (epollFd < 0)
        {
            perror("epoll_create failed");
            return;
        }
        int index = 0;
        bool mainMonitorRead = (argv[2] == "1");
        Conn firstConn(sockFd, epollFd, true);
        SetNotBlock(sockFd);
        AddReadEvent(&firstConn);
        while (true)
        {
            int num = epoll_wait(epollFd, events, 2048, -1);
            if (num < 0)
            {
                perror("epoll_wait failed");
                continue;
            }
            for (int i = 0; i < num; i++)
            {
                auto *conn = (Conn *) events[i].data.ptr;
                if (conn->Fd() == sockFd)
                {  // 有客户端的连接到来了
                    LoopAccept(sockFd, 100000, [&index, mainMonitorRead, epollFd](int clientFd)
                    {
                        SetNotBlock(clientFd);
                        if (mainMonitorRead)
                        {
                            Conn *conn = new Conn(clientFd, epollFd, true);
                            AddReadEvent(conn);  // 在MainReactor线程中监听可读事件
                        } else
                        {
                            addToSubReactor(index, clientFd);
                        }
                    });
                    continue;
                }
                // 客户端有数据可读，则把连接迁移到SubReactor线程中管理
                ClearEvent(conn, false);
                addToSubReactor(index, conn->Fd());
                delete conn;
            }
        }
    }

    void SubReactor(int threadId)
    {
        epoll_event events[2048];
        int epollFd = epoll_create(1024);
        if (epollFd < 0)
        {
            perror("epoll_create failed");
            return;
        }
        EpollFd[threadId] = epollFd;
        subReactorNotifyReady();
        while (true)
        {
            int num = epoll_wait(epollFd, events, 2048, -1);
            if (num < 0)
            {
                perror("epoll_wait failed");
                continue;
            }
            for (int i = 0; i < num; i++)
            {
                auto *conn = (Conn *) events[i].data.ptr;
                auto releaseConn = [&conn]()
                {
                    ClearEvent(conn);
                    delete conn;
                };
                if (events[i].events & EPOLLIN)
                {  // 可读
                    if (not conn->Read())
                    {          // 执行非阻塞读
                        releaseConn();
                        continue;
                    }
                    if (conn->OneMessage())
                    {             // 判断是否要触发写事件
                        ModToWriteEvent(conn);  // 修改成只监控可写事件
                    }
                }
                if (events[i].events & EPOLLOUT)
                {  // 可写
                    if (not conn->Write())
                    {          // 执行非阻塞写
                        releaseConn();
                        continue;
                    }
                    if (conn->FinishWrite())
                    {  // 完成了请求的应答写，则可以释放连接
                        releaseConn();
                    }
                }
            }
        }
    }

    class ReactorServer
    {
    private:
        uint32_t _subReactorCnt;
        String _host;
        uint16 _port;
        uint32_t _mainReactorCnt{};
        std::atomic_bool running{false};

    public:
        explicit ReactorServer(String host, uint16 port, uint32_t subReactorCnt = System::cpuNumber(),uint32_t mainReactorCnt = 2) : _host(std::move(host)),
                                                                                              _port(port), _subReactorCnt(subReactorCnt), _mainReactorCnt(mainReactorCnt)
        {};

        void start()
        {
            this->running = true;
            EpollFd = new int[this->_subReactorCnt];
            for (int i = 0; i < this->_subReactorCnt; i++)
            {
                std::thread(cpptools::reactor::SubReactor, i).detach();  // 这里需要调用detach，让创建的线程独立运行
            }
            std::array<String, 3> argv = {this->_host, std::to_string(this->_port), "1"};
            for (int i = 0; i < _mainReactorCnt; i++)
            {
                std::thread(cpptools::reactor::MainReactor, argv).detach();  // 这里需要调用detach，让创建的线程独立运行
            }
            while (this->running) sleep(1);  // 主线程陷入死循环
        }

        void stop()
        {
            this->running = false;
        }
    };
}

