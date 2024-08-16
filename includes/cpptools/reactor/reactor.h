#pragma once

#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "cpptools/common/system.h"
#include "cpptools/reactor/epollctl.hpp"

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
        index %= cpptools::common::System::cpuNumber();
        // 轮询的方式添加到SubReactor线程中
        auto *conn = new Conn(clientFd, EpollFd[index], true);
        AddReadEvent(conn);  // 监听可读事件
    }

    void MainReactor(char *argv[])
    {
        waitSubReactor();  // 等待所有的SubReactor线程都启动完毕
        int sockFd = CreateListenSocket(argv[1], atoi(argv[2]), true);
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
        bool mainMonitorRead = (std::string(argv[3]) == "1");
        Conn conn(sockFd, epollFd, true);
        SetNotBlock(sockFd);
        AddReadEvent(&conn);
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
}

