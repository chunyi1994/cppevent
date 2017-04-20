#ifndef TCPADDRESS
#define TCPADDRESS
#include <string>
namespace net {

const std::size_t TCP_DEFAULT_ADDR_PORT = std::string::npos;

struct TcpAddress {
public:
    explicit TcpAddress(const std::string& ip, std::size_t portarg = TCP_DEFAULT_ADDR_PORT) :
        ip(ip), port(portarg) {}

    TcpAddress() : ip(), port(TCP_DEFAULT_ADDR_PORT) {}

    bool operator<(const TcpAddress& other) const ;

    bool operator>(const TcpAddress& other) const ;

    bool operator==(const TcpAddress& other) const;

public:
    std::string ip;

    std::size_t port;
};

inline bool TcpAddress::operator<(const TcpAddress &other) const {
    if (ip > other.ip) {
        return true;
    }
    if (port > other.port) {
        return true;
    }
    return false;
}

inline bool TcpAddress::operator>(const TcpAddress &other) const {
    if (other == *this) {
        return false;
    }
    return !operator<(other);
}

inline bool TcpAddress::operator==(const TcpAddress &other) const {
    return other.ip == ip && other.port == port;
}

} //namespace
#endif // TCPADDRESS

