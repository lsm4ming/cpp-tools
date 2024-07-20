#include "net/datagram.h"

void cpptools::net::DatagramPacket::setHost(const String &host)
{
    this->_host = host;
}

void cpptools::net::DatagramPacket::setPort(uint16 port)
{
    this->_port = port;
}

void cpptools::net::DatagramPacket::setData(char *data, uint length)
{
    this->_data = data;
    this->_length = length;
}

String cpptools::net::DatagramPacket::getHost() const
{
    return this->_host;
}

uint16 cpptools::net::DatagramPacket::getPort() const
{
    return this->_port;
}

char *cpptools::net::DatagramPacket::getData() const
{
    return this->_data;
}

uint cpptools::net::DatagramPacket::getLength() const
{
    return this->_length;
}

cpptools::net::DatagramSocket::DatagramSocket(String host, uint16 port)
{
    this->_host = host;
    this->_port = port;
}

cpptools::net::DatagramSocket::~DatagramSocket()
{
    this->close();
}

int cpptools::net::DatagramSocket::bind()
{
    return this->bind(this->_host, this->_port);
}

int cpptools::net::DatagramSocket::bind(const String &host, uint16 port)
{
    if ((this->_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return -1;
    }
    this->_host = this->_host.empty() ? host : this->_host;
    this->_port = this->_port == 0 ? port : this->_port;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
    if (::bind(this->_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        ::close(this->_fd);
        return -1;
    }
    return this->_fd;
}

int cpptools::net::DatagramSocket::sendTo(char *data, uint length)
{
    DatagramPacket packet(data, length, this->_host, this->_port);
    return this->sendTo(packet);
}

int cpptools::net::DatagramSocket::sendTo(const DatagramPacket &packet)
{
    if (this->_fd < 0)
    {
        return -1;
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(packet.getPort());
    serverAddr.sin_addr.s_addr = inet_addr(packet.getHost().c_str());
    return ::sendto(this->_fd, packet.getData(), packet.getLength(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
}

int cpptools::net::DatagramSocket::receive(DatagramPacket packet)
{
    if (this->_fd < 0)
    {
        return -1;
    }
    else if (packet.getData() == nullptr)
    {
        return -1;
    }
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    if (recvfrom(this->_fd, packet.getData(), packet.getLength(), 0, (struct sockaddr *)&clientAddr, &addrLen) < 0)
    {
        return -1;
    }
    packet.setHost(inet_ntoa(clientAddr.sin_addr));
    packet.setPort(ntohs(clientAddr.sin_port));
    return 0;
}

void cpptools::net::DatagramSocket::close()
{
    ::close(this->_fd);
}
