#ifndef TCPADDRESS_H
#define TCPADDRESS_H
#include <string>

namespace cppevent{
const int DEFAULT_TCP_ADDR_PORT = -1;
class TcpAddress
{
public:
    TcpAddress(const std::string& ip, int port = DEFAULT_TCP_ADDR_PORT) :
        ip_(ip), port_(port)
    {}

    const TcpAddress& operator=(const TcpAddress& other)
    {
        ip_ = other.ip_;
        port_ = other.port_;
        return *this;
    }

    TcpAddress() :
        ip_(), port_(DEFAULT_TCP_ADDR_PORT)
    {}

    std::string ip_;
    int port_;
};

}
#endif // TCPADDRESS_H

