#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <cerrno>
#include <sys/socket.h>

namespace cpptools::io
{
    class RobustIo
    {
    private:
        int _fd{-1};
        bool _block{true};

    public:
        explicit RobustIo(int fd) : _fd(fd) {}

        ~RobustIo() { close(_fd); }

        ssize_t read(char *buf, ssize_t len) const;

        ssize_t write(const char *buf, ssize_t len) const;

        int setBlock(bool block);

        void setTimeout(struct timeval tv) const;

        [[nodiscard]] bool restartAgain(int err) const;
    };
}