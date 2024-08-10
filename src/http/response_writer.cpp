#include "cpptools/http/response_writer.h"

namespace cpptools::http
{
    void HttpResponseWriter::setStatus(HttpStatus status)
    {
        this->_status = status;
    }

    HttpStatus HttpResponseWriter::getStatus() const
    {
        return this->_status;
    }

    void HttpResponseWriter::addHeader(const String &key, const String &value)
    {
        this->_headers[key].push_back(value);
    }

    void HttpResponseWriter::setHeader(const String &key, const StringList &value)
    {
        this->_headers[key] = value;
    }

    void HttpResponseWriter::setContentType(const String &value)
    {
        this->setHeader("Content-Type", {value});
    }

    void HttpResponseWriter::write(char *data, size_t length)
    {
        this->_buf.write(data, (long) length);
    }

    HttpResponseWriter::~HttpResponseWriter()
    {
        if (this->_done)
        {
            return;
        }
        // 发送数据至客户端
        // 写响应行
        this->writeRespLine();
        // 写响应头
        this->writeRespHeader();
        // 写响应体
        this->writeRespBody();
    }

    void HttpResponseWriter::writeRespLine() const
    {
        auto desc = statusToString(this->_status);
        auto code = std::to_string(this->_status);
        this->_write((char *) HTTP_VERSION, (size_t) strlen(HTTP_VERSION));
        this->_write((char *) EMPTY, (size_t) strlen(EMPTY));
        this->_write((char *) code.c_str(), code.size());
        this->_write((char *) EMPTY, (size_t) strlen(EMPTY));
        this->_write((char *) desc.c_str(), desc.size());
        this->_write((char *) WRAP, strlen(WRAP));
    }

    void HttpResponseWriter::writeRespHeader() const
    {
        for (auto &item: this->_headers)
        {
            for (auto &value: item.second)
            {
                this->writeWithKV(item.first, value);
            }
        }
    }

    void HttpResponseWriter::writeWithKV(const String &key, const String &value) const
    {
        this->_write((char *) key.data(), key.size());
        this->_write((char *) COLON, 1);
        this->_write((char *) EMPTY, 1);
        this->_write((char *) value.data(), value.size());
        this->_write((char *) WRAP, 2);
    }

    void HttpResponseWriter::writeRespBody() const
    {
        auto data = this->_buf.str();
        this->writeWithKV("Content-Length", std::to_string(data.size()));
        this->_write((char *) WRAP, 2);
        if (data.empty())
        {
            return;
        }
        this->_write((char *) data.data(), data.size());
    }

    void HttpResponseWriter::write(const String &data)
    {
        this->_buf.write(data.c_str(), (long) data.size());
    }

    off_t HttpResponseWriter::sendfile(int fd, const std::string &contentType, int file_fd, size_t count)
    {
        // 发送响应头
        if (!contentType.empty())
        {
            this->addHeader("Content-Type", contentType);
        }
        this->addHeader("Content-Length", std::to_string(count));

        // 写状态行
        this->writeRespLine();

        // 写响应头
        this->writeRespHeader();

        // 空行
        this->_write((char *) WRAP, 2);

        off_t len{};
#ifdef defined(OS_MAC)
        if (::sendfile(file_fd, fd, 0, &len, nullptr, 0) <0)
#elif defined(OS_LINUX)
        if (::sendfile(fd, file_fd, &len, count) < 0)
#endif
        {
            perror("sendfile error");
        }
        this->_done = true;
        return len;
    }
}
