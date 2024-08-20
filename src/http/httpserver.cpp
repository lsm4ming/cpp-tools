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
            poll->pollWait(-1);
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
        } else if (node == nullptr)
        {
            response.setStatus(HttpStatus::HTTP_NOT_FOUND);
            return;
        }
        request.setParams(params);
        this->router.getHandler(method, node->getPattern())(request, response);
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

    void HttpServer::addInterceptor(const HttpInterceptor *interceptor)
    {
        this->interceptors.push_back(interceptor);
        std::sort(interceptors.begin(), interceptors.end(), std::greater<>());
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

    // 匹配函数
    bool pathMatch(const std::string &pattern, const std::string &path)
    {
        auto patternComponents = cpptools::utils::Strings::split(pattern, "/");
        auto pathComponents = cpptools::utils::Strings::split(path, "/");

        size_t patternIndex = 0;
        size_t pathIndex = 0;

        while (patternIndex < patternComponents.size() && pathIndex < pathComponents.size())
        {
            const std::string &p = patternComponents[patternIndex];
            const std::string &pa = pathComponents[pathIndex];
            if (p == "*")
            {
                // 如果是 "*", 匹配一个路径组件
                patternIndex++;
                pathIndex++;
            } else if (p == "**")
            {
                // 如果是 "**", 匹配零个或多个路径组件
                patternIndex++;
                if (patternIndex == patternComponents.size())
                {
                    return true; // "**" 是最后的组件，匹配成功
                }
                while (pathIndex < pathComponents.size())
                {
                    if (pathMatch(patternComponents[patternIndex], pathComponents[pathIndex]))
                    {
                        return true;
                    }
                    pathIndex++;
                }
                return false;
            } else
            {
                // 普通组件匹配
                if (p == pa)
                {
                    patternIndex++;
                    pathIndex++;
                } else
                {
                    return false;
                }
            }
        }
        return patternIndex == patternComponents.size() && pathIndex == pathComponents.size();
    }

    ssize_t HttpProtocolHandler::onRead(const PollConn &conn)
    {
        cpptools::log::LOG_DEBUG("开始处理请求，fd=%d", conn.getFd());
        Vector<char> buff;
        int index;
        ssize_t bytes_read = 0;
        while (true)
        {
            char data[BUFFER_SIZE];
            ssize_t len = conn.read(data, BUFFER_SIZE);
            if (len <= 0)
            {
                return len;
            }
            bytes_read += len;
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
                return 0;
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
            return 0;
        }
        Function<size_t(char *, size_t)> writerFun = [ObjectPtr = &conn](auto &&PH1, auto &&PH2)
        { return ObjectPtr->write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };
        {
            HttpResponseWriter responseWriter(writerFun);

            bool next = false;
            bool abort = false;
            auto nextFunc = [&next]()
            {
                next = true;
            };
            auto abortFunc = [&abort]()
            {
                abort = true;
            };
            InterceptContext context{&request, &responseWriter, nextFunc, abortFunc};
            // 先执行拦截器
            for (auto &interceptor: this->pServer->interceptors)
            {
                // 正则匹配路径
                for (auto &path: interceptor->interceptPath())
                {
                    if (!pathMatch(path, request.path))
                    {
                        continue;
                    }
                    interceptor->interceptHandler(context);
                    break;
                }
                if (abort) // 说明不需要往下执行了
                {
                    goto DONE;
                } else if (!next) // 说明不需要执行后面的拦截器
                {
                    break;
                }
            }
            this->pServer->dispatch(conn.getFd(), request, responseWriter);
        }
        DONE:
        return bytes_read;
    }

    void HttpProtocolHandler::onWrite(const PollConn &conn)
    {
        do
        {
            if (conn.finished())
            {
                // conn.flush();
                conn.close();
                return;
            }
            ssize_t ret = conn.writeConn();
            if (ret < 0)
            {
                if (EINTR == errno) continue;
                if (EAGAIN == errno && EWOULDBLOCK == errno) return;
                cpptools::log::LOG_ERROR("write failed, fd=%d", conn.getFd());
                return;
            }
        } while (true);
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


