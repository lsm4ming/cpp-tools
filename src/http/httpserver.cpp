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
        if (!dir.empty() && dir.at(0) == '/')
        {
            this->_staticDir = dir.substr(1);
        }
    }

    void HttpServer::start()
    {
        if (this->_host.empty() || this->_port == 0)
        {
            throw std::runtime_error("host or port is empty");
        }
        this->running = true;
        cpptools::net::ServerSocket serverSocket(this->_host, this->_port);
        serverSocket.setBlocking(false);
        serverSocket.setReuseport(true);
        serverSocket.setReuseaddr(true);
        serverSocket.bind();
        serverSocket.listen();

        auto wrapper = cpptools::net::HandlerWrapper::makeWrapper(new HttpProtocolHandler(this));
        auto poll = cpptools::net::createPollEvent(wrapper.get());
        poll->makeup(serverSocket.getFd());
        while (this->running)
        {
            poll->pollWait(1000);
        }
    }

    void HttpServer::stop()
    {
        this->running = false;
    }

    void HttpServer::addRoute(HttpMethod method, const std::string &path, const RouteHandler &handler)
    {
        this->router.addRoute(methodToString(method), (!path.empty() && path.at(0) == '/') ? path : "/" + path,
                              handler);
    }

    void HttpProtocolHandler::onAccept(const PollConn &conn)
    {
        // nop
    }

    int indexOf(const String &buff, const String &key)
    {
        size_t index = buff.find(key);
        return index == std::string::npos ? -1 : static_cast<int>(index);
    }

    void badRequest(const PollConn &conn)
    {
        conn.write(BAD_REQUEST, strlen(BAD_REQUEST));
        conn.close();
    }

    void HttpProtocolHandler::onRead(const PollConn &conn)
    {
        Vector<char> buff;
        int index;
        while (true)
        {
            char data[BUFFER_SIZE];
            size_t len = conn.read(data, BUFFER_SIZE);
            buff.insert(buff.end(), data, data + len);
            // 是否存在\r\n\r\n
            if (buff.size() >= 4 && (index = indexOf(String(data), HEADER_END)) != -1)
            {
                break;
            }
            if (len < BUFFER_SIZE)
            {
                // 已经读完了都不存在，说明不是合法的HTTP请求
                badRequest(conn);
                return;
            }
        }
        // 从index开始，后面都是body
        String body = String(buff.begin() + index + 4, buff.end());
        String header = String(buff.begin(), buff.begin() + index);

        // 解析请求行
        int lineIndex = indexOf(header, "\r\n");
        if (lineIndex == -1)
        {
            badRequest(conn);
            return;
        }
        String line = header.substr(0, lineIndex);
        // 根据空格分隔字符串
        StringList line_match_result = cpptools::utils::split(line, " ");
        HttpMethod method = stringToMethod(line_match_result[0]);
        String url = line_match_result[1];
        String version = line_match_result[2];

        Function<size_t(char *, size_t)> fun = [ObjectPtr = &conn](auto &&PH1, auto &&PH2)
        { return ObjectPtr->read(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };
        Request request(fun, method, url, version);
        // this->pServer->router.getHandler(conn, header, body);
        conn.close();
    }

    void HttpProtocolHandler::onWrite(const PollConn &conn)
    {
        // nop
    }

    void HttpProtocolHandler::onClose(const PollConn &conn)
    {
        // nop
    }

    HttpProtocolHandler::HttpProtocolHandler(HttpServer *pServer)
    {
        this->pServer = pServer;
    }
}


