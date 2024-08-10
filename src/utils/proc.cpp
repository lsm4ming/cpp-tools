#include "cpptools/utils/proc.h"

namespace cpptools::utils
{
    String Proc::getSelfNam()
    {
        char buf[1024] = {0};
        char *begin;
        ssize_t ret = readlink("/proc/self/exe", buf, 1023);
        if (ret == 0)
        {
            perror("readlink");
            exit(-1);
        }
        buf[ret] = 0;
        if ((begin = strrchr(buf, '/')) == nullptr) return String(buf);  //程序执行时没有前缀
        ++begin; //跳过'/'
        return String(begin);
    }
}


