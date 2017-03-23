#ifndef CONNECTION_H
#define CONNECTION_H
#include <functional>
#include <memory>
#include <vector>
#include "event.h"
#include "tcp_address.h"
#include "socket.h"
#include "buffer.h"
#include "timer.h"
#include "error_code.h"
#include "any.h"
namespace net {
class EventLoop;
class Connection : public std::enable_shared_from_this <Connection>
{
public:
    enum Status {
        eCONNECTING,
        eCLOSE,
        eERROR,
        eDELETED,
        eSIZE
    };
public:
    typedef std::shared_ptr<Connection> Pointer;
    typedef std::shared_ptr<const Connection> ConstPointer;
    typedef std::weak_ptr<Connection> WeakPointer;

    typedef std::function<void(const Pointer &)> MessageCallback;
    typedef std::function<void(const Pointer &, const ErrorCode&)> ErrorCallback;
    typedef std::function<void(const Pointer &)> CloseCallback;

public:
    Connection(EventLoop *loop, int fd);
    ~Connection();
    static Pointer create(EventLoop *loop, int fd) {
        return std::make_shared<Connection>(loop, fd);
    }
    void on_close(const CloseCallback &cb);
    void on_error(const ErrorCallback &cb);
    void on_write(const MessageCallback& cb);
    void on_read(const MessageCallback& cb);

    void set_address(const TcpAddress& addr) { peer_ = addr; }
    TcpAddress address() const { return peer_; }

    void set_status(Status status) { status_ = status; }
    void shutdown();
    //void read_once(Buffer buffer, const MessageCallback& cb);
    void send(const std::string& msg) const;
    void send(const char* msg, std::size_t n) const;
    int fd() const { return connfd_.fd(); }
    Status status() const { return status_; }
    Time last_active_time() const { return last_active_time_; }
    Time create_time() const { return create_time_; }
    time_t alive_time () const;
    const RingBuffer& buf() const { return recv_buffer_; }

    template <class T>
    void set_context(const T& t) { context_ = t; }


    Any get_context() const {
        return context_;
    }

    const std::type_info& context_type() const { return context_.type(); }

private:
    void handle_close();
    void handle_read();
    void handle_write();
    void handle_error();

private:
    Socket connfd_;
    mutable Event event_;
    Status status_;
    TcpAddress peer_;
    bool read_once_;
    MessageCallback message_callback_;
    MessageCallback write_callback_;
    ErrorCallback error_callback_;
    CloseCallback close_callback_;
    Time create_time_;
    Time last_active_time_;
    mutable RingBuffer send_buffer_;
    mutable RingBuffer recv_buffer_;
    Any context_;
};

class ConnectionHolder {
public:
    typedef std::shared_ptr<ConnectionHolder> Pointer;
    typedef std::weak_ptr<ConnectionHolder> WeakPointer;
    static Pointer create(Connection::Pointer p) {
        return std::make_shared<ConnectionHolder>(p);
    }
    explicit ConnectionHolder(Connection::Pointer p) : ptr(p) {}
    ~ConnectionHolder() {
        Connection::Pointer conn = ptr.lock();
        if (conn) {
            conn->shutdown();
        }
    }
private:
    Connection::WeakPointer ptr;
};

} //namespace
#endif // CONNECTION_H
