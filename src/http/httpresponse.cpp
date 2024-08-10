#include "cpptools/http/httpresponse.h"

namespace cpptools::http
{
    int HttpResponse::statusCode() const
    {
        return this->_code;
    }

    void HttpResponse::readResponse(int fd)
    {
        // 读取回复
        OsStringStream responseStream;
        char buffer[BUFFER_SIZE];
        int bytesReceived;

        // 读取响应头
        String headers;
        while ((bytesReceived = read(fd, buffer, 1)) > 0)
        {
            headers.append(buffer, 1);
            if (buffer[0] == '\n')
            {
                // 当读取到两个连续的换行符时表示找到了响应头的结束
                if (headers.substr(headers.length() - 4) == "\r\n\r\n")
                {
                    break;
                }
            }
        }

        std::istringstream headersStream(headers);
        String headerLine;

        // 获取响应码和描述
        std::getline(headersStream, headerLine);
        size_t first_space = headerLine.find(' ');
        size_t second_space = headerLine.find(' ', first_space + 1);
        this->_code = std::stoi(headerLine.substr(first_space + 1, second_space - first_space - 1));
        this->_status = headerLine.substr(second_space + 1);
        this->_status.pop_back();

        // 解析响应头
        while (std::getline(headersStream, headerLine) && headerLine != "\r")
        {
            size_t index = headerLine.find(':');
            String key = headerLine.substr(0, index);
            String value = headerLine.substr(index + 1);
            value.pop_back();
            this->_header[key] = {value};
            if ("Content-Length" == key)
            {
                this->_length = std::stoi(value);
            }
        }

        // 读取响应体
        if (this->_length > 0)
        {
            size_t remaining = this->_length;
            while (remaining > 0 &&
                   (bytesReceived = read(fd, buffer, std::min(remaining, size_t(BUFFER_SIZE - 1)))) > 0)
            {
                buffer[bytesReceived] = '\0';
                responseStream << buffer;
                remaining -= bytesReceived;
            }
            if (bytesReceived < 0)
            {
                perror("recv");
                return;
            }
        }
        this->_body = responseStream.str();
    }

    String HttpResponse::getBody() const
    {
        return this->_body;
    }

    String HttpResponse::httpStatus() const
    {
        return this->_status;
    }
}