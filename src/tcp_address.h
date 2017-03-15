#ifndef TCPADDRESS
#define TCPADDRESS
#include <string>
namespace net {

const int TCP_DEFAULT_ADDR_PORT = -1;
struct TcpAddress {
public:
    TcpAddress(const std::string& ip, int portarg = TCP_DEFAULT_ADDR_PORT) :
        ip(ip), port(portarg) {}

    TcpAddress() : ip(), port(TCP_DEFAULT_ADDR_PORT) {}

    std::string ip;
    int port;
};
} //namespace
#endif // TCPADDRESS

