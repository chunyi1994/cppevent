#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <set>
#include <vector>
#include "connection.h"
#include "listener.h"
#include "timer.h"
#include "ring_list.h"
namespace net {

class TcpServer {
public:
    typedef std::function<void(const Connection::ConstPointer&)> ConnectionCallback;
    typedef std::function<void(const Connection::ConstPointer &)> MessageCallback;
    typedef Connection::ErrorCallback ErrorCallback;
    typedef Connection::CloseCallback CloseCallback;
public:
    TcpServer(EventLoop *loop, std::size_t port);
    ~TcpServer();
    void start();
    void on_message(const MessageCallback &cb);
    void on_connection(const  ConnectionCallback& cb);
    void on_error(const ErrorCallback& cb);
    void on_close(const CloseCallback& cb);
    void shutdown(Connection::Pointer conn);
    time_t worked_time() const;
    std::size_t size() const { return connections_.size(); }
    std::size_t port() const { return port_; }
    void set_heartbeat_time(time_t sec);

private:
    void enable_clean_useless_conn();
private:
    EventLoop *loop_;
    std::size_t port_;
    Listener listener_;
    std::set<Connection::Pointer> connections_;    //<fd, Connection::Pointer>
    std::vector<char> buf_;
    Time create_time_;
    Time heartbeat_time_;
    RingList<std::set<ConnectionHolder::Pointer>> heartbeat_pool_;
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
    ErrorCallback error_callback_;
    CloseCallback close_callback_;


};

} // namespace
#endif // TCPSERVER_H
