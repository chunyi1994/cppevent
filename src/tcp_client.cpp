#include "tcp_client.h"
namespace net {

TcpClient::TcpClient(EventLoop *loop) :
    loop_(loop),
    conn_(nullptr),
    connector_(loop)
{
}

int TcpClient::connect(const std::string &ip, std::size_t port,  TcpClient::ConnectionCallback cb) {
    conn_ = connector_.connect(ip, port);
    if (!conn_) {
        return -1;
    }
    if (cb) {
        cb(conn_);
    }
    return 0;
}

} //namespace
