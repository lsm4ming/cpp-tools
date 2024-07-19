#include <iostream>
#include "io/file.h"
#include "time/time.h"
#include "io/reader.h"
#include "io/writer.h"

int main()
{
    cpptools::io::File f("Makefile");

    // f.mkdirs();
    std::cout << f.exists() << std::endl;
    std::cout << f.lastModified() << std::endl;
    std::cout << f.length() << std::endl;
    std::cout << f.getName() << std::endl;
    std::cout << f.getParent() << std::endl;
    std::cout << f.getAbsolutePath() << std::endl;

    cpptools::time::setTimeZone(cpptools::time::UTCPlus8);

    auto now = cpptools::time::Time::now();
    // 当前时间戳
    std::cout << now.unix() << std::endl;
    // 格式化输出
    String formatStr = now.format("%Y-%m-%d %H:%M:%S");
    std::cout << formatStr << std::endl;

    now.addDate(0, 0, 1);
    std::cout << now.format("%Y-%m-%d %H:%M:%S") << std::endl;

    std::cout << now.dayOfWeek() << std::endl;
    std::cout << now.weekOfYear() << std::endl;

    // 读取文件
    auto reader = cpptools::io::FileReader(f);
    while (!reader.eof())
    {
        std::cout << reader.readLine() << std::endl;
    }

    auto wirter = cpptools::io::FileWriter("/home/lsm/cpp-tools/hello.txt");
    wirter.write("hello world", 11);
    return 0;
}
