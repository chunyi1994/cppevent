#ifndef CONNECTION_H
#define CONNECTION_H
#include <memory>

#include "socket.h"
#include "event.h"
#include "buffer.h"
#include "tcp_address.h"
//说明: Connection持有RAII的Socket类, 在析构Connection时,也会close文件描述符
namespace cppevent{

class EventLoop;
class Connection;

typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::function<void(const ConnectionPtr &)> MessageCallback;
typedef std::function<void(const ConnectionPtr &)> ConnectionCallback;
typedef std::function<void(int)> CloseCallback;

class Connection : public std::enable_shared_from_this <Connection>{

public:
    Connection(EventLoop *loop, int fd);
    ~Connection();
    void setMessageCallback(const MessageCallback &cb);
    void setConnectionCallback(const ConnectionCallback &cb);
    void setWriteCallback(const MessageCallback& cb);
    int fd() const;
    bool connecting() const;
    std::string read(size_t len);
    std::string readAll();
    bool readLine(std::string& line);
    bool  readLine(std::string &str, char br);
    size_t readSize() const;
    void send(const std::string& msg);
    void setConnectionStatus(bool);
    void shutdown();
    const TcpAddress& address() const;
    void setAddress(const TcpAddress& addr);

private:
    void handleClose();
    void handleRead();
    void handleError();
    void handleWrite();

    Socket connfd_;
    Event event_;
    bool connecting_;
    Buffer buffer_;
    TcpAddress peer_;
    size_t bufferMaxSize_;
    MessageCallback messageCallback_;
    MessageCallback writeCallback_;
    ConnectionCallback connectionCallback_;
};
}

#endif // CONNECTION_H
