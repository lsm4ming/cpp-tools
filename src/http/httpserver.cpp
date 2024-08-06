#include "http/httpserver.h"

namespace cpptools::http
{
    void HttpServer::setHost(const String &host)
    {
        this->_host = host;
    }

    void HttpServer::setPort(uint16 port)
    {
        this->_port = port;
    }

    void HttpServer::setStaticDir(const String &dir)
    {
        this->_staticDir = dir;
    }

    void HttpServer::start()
    {
    }

    void HttpServer::stop()
    {
    }
}


