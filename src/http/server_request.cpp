#include "cpptools/http/server_request.h"

namespace cpptools::http
{
    String Request::getParam(const String &key) const
    {
        auto it = _params.find(key);
        if (it != _params.end())
        {
            return it->second;
        }
        return "";
    }

    String Request::getQuery(const String &key) const
    {
        auto values = getQuerys(key);
        if (!values.empty())
        {
            return values[0];
        }
        return "";
    }

    StringList Request::getQuerys(const String &key) const
    {
        auto it = _query.find(key);
        if (it != _query.end())
        {
            return it->second;
        }
        return {};
    }

    String Request::getHeader(const String &key) const
    {
        auto values = getHeaders(key);
        if (!values.empty())
        {
            return values[0];
        }
        return "";
    }

    StringList Request::getHeaders(const String &key) const
    {
        auto it = _header.find(key);
        if (it != _header.end())
        {
            return it->second;
        }
        return {};
    }

    Header Request::getHeader() const
    {
        return _header;
    }

    Header Request::getQuery() const
    {
        return this->_query;
    }

    void Request::setQuery(Header query)
    {
        this->_query = std::move(query);
    }

    void Request::setHeader(Header query)
    {
        this->_header = std::move(query);
    }

    void Request::setParams(SortMap<String, String> params)
    {
        this->_params = std::move(params);
    }

    size_t Request::readBody(char *data, size_t length)
    {
        if (this->_length <= 0)
        {
            return this->readFun(data, length);
        } else
        {
            // 说明预读body还有未读完的内容
            size_t len = std::min(this->_length, length);
            memcpy(data, this->_body, len);
            this->_length -= len;
            this->_body += len;
            if(len < length)
            {
                // 说明预读完了的同时，还需要读fd的内容
                return this->readFun(data + len, length - len) + len;
            }
            return len;
        }
    }

    size_t Request::getContentLength() const
    {
        auto it = _header.find("Content-Length");
        if (it != _header.end())
        {
            return std::stoi(it->second[0]);
        }
        return 0;
    }
}