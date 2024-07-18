#include <iostream>
#include "file/file.h"
#include "time/time.h"

int main()
{
    cpptools::file::File f("/home/lsm/cpp-tools/hello.txt");

    // f.mkdirs();
    std::cout << f.exists() << std::endl;
    std::cout << f.lastModified() << std::endl;
    std::cout << f.length() << std::endl;

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
    return 0;
}
