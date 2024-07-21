#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <utility>
#include "common/common.h"

using namespace cpptools::common;

namespace cpptools::net
{
    class DatagramPacket
    {
    private:
        mutable char *_data{};
        mutable uint _length{};
        String _host;
        uint16 _port{};

    public:
        DatagramPacket() = default;

        explicit DatagramPacket(char *data, uint length) : _data(data), _length(length){};

        DatagramPacket(char *data, uint length, String host, uint16 port) : _data(data), _length(length), _host(std::move(host)), _port(port){};

        void setHost(const String &host);

        void setPort(uint16 port);

        void setData(char *data, uint length);

        String getHost() const;

        uint16 getPort() const;

        char *getData() const;

        uint getLength() const;
    };

    class DatagramSocket
    {
    private:
        String _host;
        uint16 _port{};
        int _fd{-1};
        bool _connected{false};

    public:
        DatagramSocket() = default;

        DatagramSocket(String host, uint16 port);

        ~DatagramSocket();

        int bind();

        int bind(const String &host, uint16 port);

        int sendTo(char *data, uint length);

        int sendTo(const DatagramPacket &packet);

        int receive(DatagramPacket packet);

        void close() const;
    };
}