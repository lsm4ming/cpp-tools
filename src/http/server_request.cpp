#include "http/server_request.h"

#include <utility>

namespace cpptools::http
{

    String Request::getParam(const String &key) const
    {
        return params.at(key);
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
        return _query.at(key);
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
        return _header.at(key);
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
        this->params = std::move(params);
    }
}