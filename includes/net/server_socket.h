#include "socket.h"
#include "inet_address.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class ServerSocket : public Socket
    {
    public:
        ServerSocket() = default;

        ServerSocket(String host, uint16 port) : Socket(std::move(host), port){};

        int bind();

        int listen(int n = 16);

        int accept(InetAddress &address);
    };
}