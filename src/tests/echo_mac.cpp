#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <cstring>

#define MAX_EVENTS 10
#define PORT 10040
#define BUFFER_SIZE 1024

// 设置套接字为非阻塞
int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int server_fd, kq;
    struct sockaddr_in address{};
    struct kevent event{};
    struct kevent events[MAX_EVENTS];
    int addr_len = sizeof(address);

    // 创建服务器套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字为非阻塞
    if (setNonBlocking(server_fd) < 0)
    {
        perror("setNonBlocking");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听套接字
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 创建 kqueue 实例
    kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    // 添加服务器套接字到 kqueue
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1)
    {
        perror("kevent: server_fd");
        exit(EXIT_FAILURE);
    }

    std::unordered_map<int, std::string> client_buffers;

    while (true)
    {
        int n_fds = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (n_fds == -1)
        {
            perror("kevent");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n_fds; ++i)
        {
            if (events[i].ident == (uintptr_t) server_fd)
            {
                int client_fd;
                // 处理新的连接
                while ((client_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addr_len)) > 0)
                {
                    if (setNonBlocking(client_fd) < 0)
                    {
                        perror("setNonBlocking");
                        continue;
                    }
                    EV_SET(&event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1)
                    {
                        perror("kevent: client_fd READ");
                        close(client_fd);
                    }
                    client_buffers[client_fd] = ""; // 初始化缓冲区
                }
                if (client_fd == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
                {
                    perror("accept");
                }
            } else
            {
                int client_fd = (int) events[i].ident;
                std::cout << "client_fd=" << client_fd << std::endl;

                if (events[i].flags & EV_EOF)
                {
                    // 客户端断开连接
                    std::cout << "Client disconnected" << std::endl;
                    close(client_fd);
                    client_buffers.erase(client_fd);
                } else if (events[i].filter == EVFILT_READ)
                {
                    // 处理读事件
                    char buffer[BUFFER_SIZE];
                    ssize_t bytes_read;
                    while ((bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
                    {
                        // 累积数据到缓冲区
                        client_buffers[client_fd] = "HTTP/1.1 200 OK" + std::string("\r\n") +
                                                    "Content-Type: application/json; charset=utf-8" +
                                                    std::string("\r\n") +
                                                    "Content-Length: 51" + std::string("\r\n\r\n");
                        client_buffers[client_fd] += R"({"code":200,"data":"pong","message":"操作成功"})";
                        // 注册写事件以发送数据
                        EV_SET(&event, client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1)
                        {
                            perror("kevent: client_fd WRITE");
                            close(client_fd);
                            client_buffers.erase(client_fd);
                        }
                    }

                    if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        perror("recv");
                        close(client_fd);
                        client_buffers.erase(client_fd);
                    } else if (bytes_read == 0)
                    {
                        std::cout << "Client disconnected" << std::endl;
                        close(client_fd);
                        client_buffers.erase(client_fd);
                    }
                } else if (events[i].filter == EVFILT_WRITE)
                {
                    // 处理写事件
                    std::string &buffer = client_buffers[client_fd];
                    ssize_t bytes_written = send(client_fd, buffer.c_str(), buffer.size(), 0);
                    if (bytes_written == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        perror("send");
                        close(client_fd);
                        client_buffers.erase(client_fd);
                    } else if (bytes_written > 0)
                    {
                        buffer.erase(0, bytes_written); // 从缓冲区中移除已发送的数据

                        if (buffer.empty())
                        {
                            // 数据发送完毕，注销写事件
                            EV_SET(&event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            if (kevent(kq, &event, 1, nullptr, 0, nullptr) == -1)
                            {
                                perror("kevent: client_fd READ");
                                close(client_fd);
                                client_buffers.erase(client_fd);
                            }
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}