#include <iostream>
#include "io/file.h"
#include "time/time.h"
#include "io/reader.h"
#include "io/writer.h"
#include "net/socket.h"
#include "net/datagram.h"
#include "json/jsonparse.h"
#include "http/httpclient.h"
#include "http/httpserver.h"
#include "net/poll_channel.h"
#include "log/log.h"

void ioTest()
{
    cpptools::io::File f("Makefile");

    // f.mkdirs();
    std::cout << f.exists() << std::endl;
    std::cout << f.lastModified() << std::endl;
    std::cout << f.length() << std::endl;
    std::cout << f.getName() << std::endl;
    std::cout << f.getParent() << std::endl;
    std::cout << f.getAbsolutePath() << std::endl;

    // 读取文件
    auto reader = cpptools::io::FileReader(f);
    while (!reader.eof())
    {
        std::cout << reader.readLine() << std::endl;
    }

    // 写文件
    auto writer = cpptools::io::FileWriter("/home/lsm/cpp-tools/hello.txt");
    writer.write("hello world", 11);
}

void timeTest()
{
    std::cout << "本地时区" << cpptools::time::getLocalTimeZone() << std::endl;

    cpptools::time::setTimeZone(cpptools::time::UTCPlus8);

    auto now = cpptools::time::Time::now();
    // 当前时间戳
    std::cout << now.unixSecond() << std::endl;

    // 格式化输出
    std::cout << now.format("%Y-%m-%d %H:%M:%S") << std::endl;

    now.addDate(0, 0, 1);
    std::cout << now.format("%Y-%m-%d %H:%M:%S") << std::endl;

    std::cout << now.dayOfWeek() << std::endl;
    std::cout << now.weekOfYear() << std::endl;

    now = cpptools::time::Time::now();
    now = now.add(24 * cpptools::time::HOUR);
    std::cout << now.format("%Y-%m-%d %H:%M:%S") << std::endl;
}

void netTest()
{
    auto socket = new cpptools::net::Socket();
    socket->connect("157.148.69.74", 80);

    auto udpSocket = new cpptools::net::DatagramSocket();
    udpSocket->bind("127.0.0.1", 9900);
    char buf[16] = "hello";
    std::cout << "udpSocket sendTo result:" << udpSocket->sendTo(buf, sizeof(buf)) << std::endl;
}

cpptools::json::JsonValue getTestValue()
{
    auto jsonValue = cpptools::json::JsonValue();
    jsonValue["name"] = "lsm";
    jsonValue["age"] = 30;
    return jsonValue;
}

void jsonTest()
{
    auto jsonValue = getTestValue();
    std::cout << jsonValue.getType() << std::endl;
    auto name = jsonValue["name"];
    std::cout << name.getType() << std::endl;


    auto objectValue = cpptools::json::JsonParse::parse(R"(  {"a": 1, "b": 2.5 , "c":"hello"})");
    std::cout << objectValue.getType() << std::endl;

    auto arrayValue = cpptools::json::JsonParse::parse(R"(  [1, 2.5, "hello"])");
    std::cout << arrayValue.getType() << std::endl;

    std::cout << objectValue << std::endl;

    std::cout << arrayValue << std::endl;

    std::cout << jsonValue << std::endl;
}

void httpClientTest()
{
    cpptools::http::HttpClient httpClient;
    auto response = httpClient.get("https://www.baidu.com");
    std::cout << response.getBody() << std::endl;
}

/**
 * 实现一个群聊
 */
class chatHandler : public cpptools::net::ConnectHandler
{
    HashSet<const cpptools::net::PollConn *> connSet;

public:
    void onAccept(const cpptools::net::PollConn &conn) override
    {
        std::cout << "连接加入..." << std::endl;
        connSet.insert(&conn);
    }

    void onRead(const cpptools::net::PollConn &conn) override
    {
        // 读取所有的内容
        while (true)
        {
            char buff[1024];
            auto n = conn.read(buff, sizeof(buff));
            // 连接是否退出
            if (n == 0)
            {
                this->onClose(conn);
                break;
            }
            buff[n] = '\0';
            std::cout << buff << std::endl;
            for (auto c: connSet)
            {
                c->write(buff, n);
            }
            if (n < 1024)
            {
                break;
            }
        }
    }

    void onWrite(const cpptools::net::PollConn &conn) override
    {
        std::cout << "onWrite" << std::endl;
    }

    void onClose(const cpptools::net::PollConn &conn) override
    {
        connSet.erase(&conn);
        std::cout << "连接退出" << std::endl;
    }
};

void pollTest()
{
    std::cout << "pollTest" << std::endl;
    volatile std::atomic_bool running = true;
    cpptools::net::ServerSocket serverSocket("127.0.0.1", 9999);
    serverSocket.setBlocking(false);
    serverSocket.setReuseaddr(true);
    serverSocket.setTcpNoDelay(true);
    serverSocket.setReuseport(true);
    serverSocket.setKeepalive(true);

    auto wrapper = cpptools::net::HandlerWrapper::makeWrapper(new chatHandler);

    auto poll = cpptools::net::createPollEvent(wrapper.get());
    if (serverSocket.bind() < 0)
    {
        throw std::runtime_error("bind error");
    }
    if (serverSocket.listen() < 0)
    {
        throw std::runtime_error("listen error");
    }
    if (poll->makeup(serverSocket.getFd()) < 0)
    {
        throw std::runtime_error("makeup error");
    }

    while (running)
    {
        poll->pollWait(1000);
    }
}

void httpServerTest()
{
    cpptools::http::HttpServer server("127.0.0.1", 9999);
    server.addRoute(cpptools::http::HttpMethod::HTTP_GET, "/",
                    [](const cpptools::http::Request &req, cpptools::http::HttpResponseWriter &resp)
                    {
                        auto t = req.getQuery("name");
                        resp.addHeader("Content-Type", "text/plain");
                        auto result = "hello: " + t;
                        resp.write(result.data(), result.length());
                    });
    server.start();
}

int main()
{
    cpptools::log::Logger::getInstance().log(cpptools::log::ERROR, "",
                                             1, "", "hello :%s", (char *) "lsm");

    cpptools::log::LOG_LEVEL(cpptools::log::DEBUG);
    cpptools::log::LOG_FILENAME("我的日志%Y-%m-%d{level}.log");

    cpptools::log::LOG_ERROR("hello :%s", "lsm");
    // ioTest();
    timeTest();
    netTest();
    jsonTest();
    httpClientTest();
    // pollTest();
    httpServerTest();
    return 0;
}
