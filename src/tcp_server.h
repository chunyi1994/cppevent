#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <set>
#include <vector>
#include "connection.h"
#include "listener.h"
#include "timer.h"
#include "ring_list.h"
#include "event_loop.h"
namespace net {

class TcpServer {
public:
    typedef std::function<void(const Connection::Pointer&)> ConnectionCallback;
    typedef std::function<void(const Connection::Pointer &)> MessageCallback;
    typedef Connection::ErrorCallback ErrorCallback;
    typedef Connection::CloseCallback CloseCallback;
public:
    TcpServer(EventLoop *loop, std::size_t port);
    ~TcpServer();
    void start();

    //消息，连接， 错误， 关闭 的回调设置
    void on_message(const MessageCallback &cb);
    void on_connection(const  ConnectionCallback& cb);
    void on_error(const ErrorCallback& cb);
    void on_close(const CloseCallback& cb);

    //关闭连接, 会引起回调on_close
    void shutdown(Connection::Pointer conn);

    //运行时长
    time_t worked_time() const;

    //连接个数
    std::size_t size() const { return connections_.size(); }
    //监听端口
    std::size_t port() const { return port_; }
    //设置心跳时间
    void set_heartbeat_time(time_t sec);

    //发生错误的总计
    uint64_t error_nums() const { return error_nums_; }

    std::string info() const;

private:
    void enable_clean_useless_conn();

    void hanlde_close(Connection::Pointer conn);

    void process_heartbeats(Connection::Pointer conn);

    void init_heartsbeats(Connection::Pointer conn);

private:
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(const TcpServer&) = delete;

    TcpServer(TcpServer&&) = delete;

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

    uint64_t error_nums_;
};

} // namespace
#endif // TCPSERVER_H
