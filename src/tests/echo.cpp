#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define PORT 10040

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
    int server_fd, epoll_fd;
    struct epoll_event event{}, events[MAX_EVENTS];
    struct sockaddr_in address{};
    int addrlen = sizeof(address);

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

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // 添加服务器套接字到 epoll 事件中
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET; // ET 模式
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
    {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == server_fd)
            {
                // 处理新的连接
                int client_fd;
                while ((client_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) > 0)
                {
                    if (setNonBlocking(client_fd) < 0)
                    {
                        perror("setNonBlocking");
                        continue;
                    }
                    event.data.fd = client_fd;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
                    {
                        perror("epoll_ctl: client_fd");
                        close(client_fd);
                    }
                }
                if (client_fd == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
                {
                    perror("accept");
                }
            } else
            {
                // 处理客户端数据
                int client_fd = events[i].data.fd;
                char buffer[1024];
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes_read <= 0)
                {
                    if (bytes_read == 0)
                    {
                        std::cout << "Client disconnected" << std::endl;
                    } else
                    {
                        perror("recv");
                    }
                    close(client_fd);
                } else
                {
                    std::string message_ = "HTTP/1.1 200 OK" + std::string("\r\n") +
                                           "Content-Type: application/json; charset=utf-8" + std::string("\r\n") +
                                           "Content-Length: 51" + std::string("\r\n\r\n");
                    message_ += R"({"code":200,"data":"pong","message":"操作成功"})";
                    send(client_fd, message_.c_str(), message_.length(), 0);
                    close(client_fd); // 主动关闭连接
                    std::cout << "响应一次" << std::endl;
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
