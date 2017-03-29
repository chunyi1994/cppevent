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
    //设置回调
    void on_close(const CloseCallback &cb);
    void on_error(const ErrorCallback &cb);
    void on_write(const MessageCallback& cb);
    void on_read(const MessageCallback& cb);

    //地址
    void set_address(const TcpAddress& addr) { peer_ = addr; }
    TcpAddress address() const { return peer_; }

    //状态
    void set_status(Status status) { status_ = status; }
    Status status() const { return status_; }

    //关闭， 并且会引起回调on_close
    void shutdown();

    //发送消息
    void send(const std::string& msg);
    void send(const char* msg, std::size_t n);

    //socket文件描述符
    int fd() const { return connfd_.fd(); }

    //上次活动时间
    Time last_active_time() const { return last_active_time_; }
    //创建的时间
    Time create_time() const { return create_time_; }
    //一共存活时间
    time_t alive_time () const;

    //环形缓存
    const RingBuffer& buf() const { return recv_buffer_; }
    RingBuffer& buf() { return recv_buffer_; }

    //Any类的get，set
    template <class T>
    void set_context(const T& t) { context_ = t; }
    Any get_context() const { return context_; }
    const std::type_info& context_type() const { return context_.type(); }

    //回调锁，用于锁住connection的回调，防止被修改。
    //如果确定要修改回调，可以解开
    bool lock() const { return callback_lock_; }
    void set_lock(bool lock) { callback_lock_ = lock; }

private:
    void handle_close();
    void handle_read();
    void handle_write();
    void handle_error();

private:
    Socket connfd_;
    Event event_;
    Status status_;
    TcpAddress peer_;
    MessageCallback message_callback_;
    MessageCallback write_callback_;
    ErrorCallback error_callback_;
    CloseCallback close_callback_;
    Time create_time_;
    Time last_active_time_;
    RingBuffer send_buffer_;
    RingBuffer recv_buffer_;
    Any context_;
    //用于锁住callback不被其他人修改
    bool callback_lock_;
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
