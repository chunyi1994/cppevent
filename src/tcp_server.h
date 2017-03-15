#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <set>
#include <vector>
#include "connection.h"
#include "listener.h"
namespace net {

class TcpServer
{
public:
    typedef std::function<void(const Connection::ConstPointer&)> ConnectionCallback;
    typedef std::function<void(const Connection::ConstPointer &, Buffer ,std::size_t)> MessageCallback;
    typedef std::function<void(const Connection::ConstPointer &)> ErrorCallback;
    typedef std::function<void(const Connection::ConstPointer &)> CloseCallback;
public:
    TcpServer(EventLoop *loop, std::size_t port);
    ~TcpServer();
    void start();
    void set_message_callback(const MessageCallback &cb);
    void set_connection_callback(const  ConnectionCallback& cb);
    void set_error_callback(const ErrorCallback& cb);
    void shutdown(Connection::Pointer conn);
private:
    EventLoop *loop_;
    std::size_t port_;
    Listener listener_;
    std::set<Connection::Pointer> connections_;    //<fd, Connection::Pointer>
    std::vector<char> buf_;
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
    ErrorCallback error_callback_;
};

} // namespace
#endif // TCPSERVER_H
