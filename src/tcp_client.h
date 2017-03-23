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
    typedef std::function<void(const Connection::ConstPointer&, ErrorCode)> ConnectionCallback;
    typedef std::function<void(const Connection::ConstPointer &)> MessageCallback;
    typedef Connection::ErrorCallback ErrorCallback;
    typedef Connection::CloseCallback CloseCallback;
public:
    TcpClient(EventLoop *loop);
    void on_connect(const std::string& ip, std::size_t port, ConnectionCallback cb);
    void on_read(const MessageCallback& cb);
    void on_close(const CloseCallback& cb);
    void on_error(const ErrorCallback& cb);
    void shutdown();
private:
    EventLoop* loop_;
    Connection::Pointer conn_;
    Connector connector_;
    std::vector<char> buf_;
    MessageCallback message_callback_;
    CloseCallback close_callback_;
    ErrorCallback error_callback_;
};

} //namespace
#endif // TCP_CLIENT_H
