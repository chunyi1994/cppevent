#ifndef CONNECTOR_H
#define CONNECTOR_H
#include "socket.h"
#include "connection.h"
namespace net {
class EventLoop;
class Connector
{
public:
    Connector(EventLoop *loop);
    Connection::Pointer connect(const std::string& ip, std::size_t port);
private:
    EventLoop *loop_;
};

}//namespace

#endif // CONNECTOR_H
