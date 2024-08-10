#include "cpptools/http/httpserver.h"

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


        this->router.displayRoute([](const String &method, const String &path, const RouteHandler &handler)
                                  {
                                      std::cout << method << " : " << path << std::endl;
                                  });

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

    void HttpServer::dispatch(int fd, Request &request, HttpResponseWriter &response)
    {
        String method = methodToString(request.method);
        RouteHandler handler;
        auto [node, params] = this->router.getRoute(method, request.path);
        if (node == nullptr && !staticDispatch(fd, request, response))
        {
            response.setStatus(HttpStatus::HTTP_NOT_FOUND);
            return;
        }
        request.setParams(params);
        this->router.getHandler(method, request.path)(request, response);
    }

    String eductionContentType(const String &path)
    {
        size_t pos = path.find_last_of('.');
        HashMap<String, String>::iterator iter;
        if (pos == String::npos ||
            (iter = CONTENT_TYPE_MAP.find(path.substr(pos + 1))) == CONTENT_TYPE_MAP.end())
        {
            return "application/octet-stream";
        }
        return iter->second;
    }

    bool HttpServer::staticDispatch(int fd, Request &request, HttpResponseWriter &response)
    {
        if (request.method != HttpMethod::HTTP_GET)
        {
            return false;
        }
        std::string path = request.path;
        if (!path.empty() && path[0] != '/')
        {
            path = '/' + path;
        }
        path = this->_staticDir + path;

        struct stat fileStat{};
        int ret = stat(path.c_str(), &fileStat);
        if (ret < 0 || S_ISDIR(fileStat.st_mode))
        {
            return false;
        }
        int static_fd = open(path.c_str(), O_RDONLY);
        response.sendfile(fd, eductionContentType(path), static_fd, fileStat.st_size);
        close(static_fd);
        return true;
    }

    void HttpProtocolHandler::onAccept(const PollConn &conn)
    {
        // nop
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
            if (buff.size() >= 4 && (index = cpptools::utils::Strings::indexOf(String(data), HEADER_END)) != -1)
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
        Function<size_t(char *, size_t)> readFun = [ObjectPtr = &conn](auto &&PH1, auto &&PH2)
        { return ObjectPtr->read(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };
        RequestParse parse(header);
        Request request(readFun, body.c_str(), body.length());
        if (parse.parse(request) == -1)
        {
            badRequest(conn);
            return;
        }
        Function<size_t(char *, size_t)> writerFun = [ObjectPtr = &conn](auto &&PH1, auto &&PH2)
        { return ObjectPtr->write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };
        {
            HttpResponseWriter responseWriter(writerFun);
            this->pServer->dispatch(conn.getFd(), request, responseWriter);
        }
        conn.flush();
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


