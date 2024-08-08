#include "http/server_request.h"

namespace cpptools::http
{

    String Request::getParams(const String &key) const
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
        return query.at(key);
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
        return header.at(key);
    }
}