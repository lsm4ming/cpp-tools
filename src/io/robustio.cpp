#include "cpptools/io/robustio.h"

namespace cpptools::io
{
    ssize_t RobustIo::read(char *buf, ssize_t len) const
    {
        ssize_t total = len;
        while (total > 0)
        {
            ssize_t ret = ::read(_fd, buf, total);
            if (ret <= 0)
            {
                return ret;
            }
            buf += ret;
            total -= ret;
        }
        return len;
    }

    ssize_t RobustIo::write(const char *buf, ssize_t len) const
    {
        ssize_t total = len;
        while (total > 0)
        {
            ssize_t ret = ::write(_fd, buf, total);
            if (ret <= 0)
            {
                return ret;
            }
            buf += ret;
            total -= ret;
        }
        return len;
    }

    int RobustIo::setBlock(bool block)
    {
        _block = block;
        return fcntl(_fd, F_SETFL, _block ? _fd | O_NONBLOCK : _fd & ~O_NONBLOCK);
    }

    void RobustIo::setTimeout(struct timeval tv) const
    {
        if (this->_block)
        {
            return;
        }
        setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    }

    bool RobustIo::restartAgain(int err) const
    {
        if (err == EINTR)
        {
            return true;
        } else if (this->_block)
        {
            return false;
        }
        return EAGAIN == err || EWOULDBLOCK == err;
    }
}


