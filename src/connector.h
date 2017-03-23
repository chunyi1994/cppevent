#ifndef CONNECTOR_H
#define CONNECTOR_H
#include "socket.h"
#include "connection.h"
#include <functional>
namespace net {
class EventLoop;
class Connector
{
public:
    typedef std::function<void(Connection::Pointer, ErrorCode)> ConnectionCallback;
public:
    Connector(EventLoop *loop);
    Connection::Pointer connect(const std::string& ip, std::size_t port);
    void connect(std::string ip, std::size_t port, ConnectionCallback cb, time_t timeout = 3);
private:
    EventLoop *loop_;
};

}//namespace

#endif // CONNECTOR_H
