#include "cpptools/utils/network.h"

namespace cpptools::utils
{
    String Network::getIp(const String &ethName)
    {
        struct ifaddrs *ifa;
        struct ifaddrs *ifList = nullptr;
        if (getifaddrs(&ifList) < 0)
        {
            return "";
        }
        char ip[20] = {0};
        String ipStr = "127.000.000.001";
        for (ifa = ifList; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr->sa_family != AF_INET)
            {
                continue;
            }
            if (strcmp(ifa->ifa_name, ethName.c_str()) == 0)
            {
                auto *addr = (struct sockaddr_in *) (ifa->ifa_addr);
                auto *sin_addr = (uint8_t *) &(addr->sin_addr);
                snprintf(ip, 20, "%03d.%03d.%03d.%03d", sin_addr[0], sin_addr[1], sin_addr[2], sin_addr[3]);
                ipStr = String(ip);
                break;
            }
        }
        freeifaddrs(ifList);
        return ipStr;
    }
}


