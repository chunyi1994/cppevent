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
    explicit Connector(EventLoop *loop);
    //阻塞式
    Connection::Pointer connect(const std::string& ip, std::size_t port);
    //回调式（目前也是阻塞的）
    void connect(std::string ip, std::size_t port, ConnectionCallback cb, time_t timeout = 3);
private:
    EventLoop *loop_;
};

}//namespace

#endif // CONNECTOR_H
