#include <iostream>
#include "file/file.h"

int main()
{
    cpptools::file::File f("/home/lsm/cpp-tools/hello.txt");

    // f.mkdirs();
    std::cout << f.exists() << std::endl;
    std::cout << f.lastModified() << std::endl;
    std::cout << f.length() << std::endl;
    return 0;
}
