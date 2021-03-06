#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <functional>
#include <map>
#include "connection.h"
#include "connector.h"
namespace net {

class TcpClientTaskChain {
public:
private:

};


class EventLoop;
class TcpClient
{
public:
    typedef std::function<void(const Connection::Pointer&, ErrorCode)> ConnectionCallback;

    typedef std::function<void(const Connection::Pointer &)> MessageCallback;

    typedef Connection::ErrorCallback ErrorCallback;

    typedef Connection::CloseCallback CloseCallback;

    typedef std::shared_ptr<TcpClient> Pointer;

public:
    explicit TcpClient(EventLoop *loop);

    Connection::Pointer connect(const TcpAddress& addr);

    void on_connect(const std::string& ip, std::size_t port, ConnectionCallback cb);

    void on_connect(const TcpAddress& addr, ConnectionCallback cb);

    void on_read(const MessageCallback& cb);

    void on_close(const CloseCallback& cb);

    void on_error(const ErrorCallback& cb);

    void remove(Connection::Pointer conn);

    void shutdown();

    std::size_t size() const { return conns_.size(); }

    std::string info() const;

public:
    static Pointer create(EventLoop* loop) { return std::make_shared<TcpClient>(loop); }

private:
    //nocopyable
    TcpClient& operator=(const TcpClient&) = delete;
    TcpClient(const TcpClient&) = delete;
    TcpClient(TcpClient&&) = delete;

private:
    EventLoop* loop_;

    std::map<int, Connection::Pointer> conns_;

  //  std::map<TcpAddress, ConnectionCallback> callbacks_;

    Connector connector_;

    std::vector<char> buf_;

    MessageCallback message_callback_;

    CloseCallback close_callback_;

    ErrorCallback error_callback_;
};

} //namespace
#endif // TCP_CLIENT_H
