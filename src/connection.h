#ifndef CONNECTION_H
#define CONNECTION_H
#include <functional>
#include <memory>
#include <vector>
#include "event.h"
#include "tcp_address.h"
#include "socket.h"
#include "buffer.h"
namespace net {
class EventLoop;
class Connection : public std::enable_shared_from_this <Connection>
{
public:
    enum Status {
        eCONNECTING,
        eCLOSE,
        eSIZE
    };
public:
    typedef std::shared_ptr<Connection> Pointer;
    typedef std::shared_ptr<const Connection> ConstPointer;

    typedef std::function<void(const Pointer &, Buffer ,std::size_t)> MessageCallback;
    typedef std::function<void(const Pointer &)> ErrorCallback;
    typedef std::function<void(const Pointer &)> CloseCallback;
public:

    Connection(EventLoop *loop, int fd);
    ~Connection();
    static Pointer create(EventLoop *loop, int fd) {
        return std::make_shared<Connection>(loop, fd);
    }
   // void set_message_callback(const MessageCallback &cb);
    void set_close_callback(const CloseCallback &cb);
    void set_error_callback(const ErrorCallback &cb);
    void set_write_callback(const MessageCallback& cb);
    void set_address(const TcpAddress& addr) { peer_ = addr; }
    void set_status(Status status) { status_ = status; }
    void shutdown();
    void read_once(Buffer buffer, const MessageCallback& cb);
    void read(Buffer buffer, const MessageCallback& cb);

    int fd() const { return connfd_.fd(); }
    Status status() const { return status_; }
    void send(const std::string& msg) const;
    void send(const char* msg, std::size_t n) const;

private:
    void handle_close();
    void handle_read();
    void handle_write();
    void handle_error();

    //std::string read(size_t len);
    //std::string read_all();
    //bool readline(std::string& line);
    //bool  readline(std::string &str, char br);
    //std::size_t read_size() const;
    //void send(const std::string& msg);
    //void set_connection_status(bool);
    //const TcpAddress& address() const;
    //void set_address(const TcpAddress& addr);
private:
    Socket connfd_;
    Event event_;
    Status status_;
    TcpAddress peer_;
    Buffer buffer_;
    bool read_once_;
    MessageCallback message_callback_;
    MessageCallback write_callback_;
    ErrorCallback error_callback_;
    CloseCallback close_callback_;
};

} //namespace
#endif // CONNECTION_H
