#include <iostream>
#include "io/file.h"
#include "time/time.h"
#include "io/reader.h"
#include "io/writer.h"
#include "net/socket.h"
#include "net/datagram.h"

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
    auto wirter = cpptools::io::FileWriter("/home/lsm/cpp-tools/hello.txt");
    wirter.write("hello world", 11);
}

void timeTest()
{
    cpptools::time::setTimeZone(cpptools::time::UTCPlus8);

    auto now = cpptools::time::Time::now();
    // 当前时间戳
    std::cout << now.unix() << std::endl;

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

int main()
{
    // ioTest();
    timeTest();
    netTest();
    return 0;
}
