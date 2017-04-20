#ifndef CONNECTOR_H
#define CONNECTOR_H
#include "socket.h"
#include "connection.h"
#include <functional>
#include <map>
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

    Connection::Pointer connect(const TcpAddress& addr);

    //回调式
    void connect(std::string ip, std::size_t port, ConnectionCallback cb);

    void connect(const TcpAddress& addr,ConnectionCallback cb);

    //超时时间
    void set_timeout(int timeout) { timeout_ = timeout;}

private:
    Connection::Pointer new_connection(int fd, const TcpAddress &addr);

    void process_in_progress_connect(int fd,
                              TcpAddress addr,
                              Connector::ConnectionCallback cb);

    void remove(Connection::Pointer conn);

    void check_connect_state(Connection::WeakPointer conn, ConnectionCallback cb);
private:
     //nocopyable
    Connector(Connector&&) = delete;
    Connector& operator=(const Connector&) = delete;
    Connector(const Connector&) = delete;
private:
    EventLoop *loop_;
    std::map<Connection::Pointer, int> conns_;
    int timeout_;
};

}//namespace

#endif // CONNECTOR_H
