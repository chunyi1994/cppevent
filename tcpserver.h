#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <map>
#include <memory>
#include <functional>
#include <vector>

#include "listener.h"
#include "connection.h"
#include "utils.h"
namespace cppevent{

class Connection;
class EventLoop;

class TcpServer
{
public:
    TcpServer(EventLoop *loop, size_t port);
    ~TcpServer();
    void start();
    void setMessageCallback(const MessageCallback &cb);
    void setConnectionCallback(const ConnectionCallback &cb);

private:
    void newConnection(int sockfd, sockaddr_in* addr, size_t size);
    void handleConnection(const ConnectionPtr& conn);
    void handleClose(int sockfd);

    EventLoop *loop_;
    size_t port_;
    Listener listener_;
    std::map<int, ConnectionPtr> connections_;    //<fd, connPtr>
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
};
}
#endif // TCPSERVER_H
