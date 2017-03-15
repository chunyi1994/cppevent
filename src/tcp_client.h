#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <functional>
#include "connection.h"
#include "connector.h"
namespace net {
class EventLoop;
class TcpClient
{
public:
    typedef std::function<void(Connection::Pointer)> ConnectionCallback;
public:
    TcpClient(EventLoop *loop);
    int connect(const std::string& ip, std::size_t port,  ConnectionCallback cb);

private:
    EventLoop* loop_;
    Connection::Pointer conn_;
    Connector connector_;
};

} //namespace
#endif // TCP_CLIENT_H
