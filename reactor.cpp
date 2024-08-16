#include "cpptools/common/system.h"
#include "cpptools/reactor/reactor.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "invalid input" << std::endl;
        std::cout << "example: ./EpollReactorThreadPoolMS 0.0.0.0 1688 1" << std::endl;
        return -1;
    }
    cpptools::reactor::EpollFd = new int[cpptools::common::System::cpuNumber()];
    for (int i = 0; i < cpptools::common::System::cpuNumber(); i++)
    {
        std::thread(cpptools::reactor::SubReactor, i).detach();  // 这里需要调用detach，让创建的线程独立运行
    }
    int mainReactorCnt = 3;
    for (int i = 0; i < mainReactorCnt; i++)
    {
        std::thread(cpptools::reactor::MainReactor, argv).detach();  // 这里需要调用detach，让创建的线程独立运行
    }
    while (true) sleep(1);  // 主线程陷入死循环
    return 0;
}