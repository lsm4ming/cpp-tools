#include "http/server_request.h"

#include <utility>

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
}