#include "http/response_writer.h"

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

    size_t HttpResponseWriter::write(char *data, size_t length) const
    {
        return ::write(this->_fd, data, length);
    }
}
