#include "listener.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "event.h"
#include "event_loop.h"
#include "string.h"
#include "utils.h"
#include "logging/logger.h"
#include "tcp_address.h"
#include "connection.h"
namespace net {
const int LISTENQ = 10;
Listener::Listener(EventLoop* loop, std::size_t port) :
    loop_(loop),
    port_(port) ,
    listen_socket_(utils::create_non_blocking_socket()),
    event_(new Event(loop, listen_socket_, EPOLLIN | EPOLLET ))
{
    event_->set_read_callback([this]() {
        sockaddr_in clientaddr;
        size_t clilen = sizeof(clientaddr);
        memset(&clientaddr, 0, sizeof(struct sockaddr_in));
        int connfd = ::accept(listen_socket_, (sockaddr *) &clientaddr, &clilen);
        if (connfd < 0){
            LOG_DEBUG<<"accept error";
        }

        net::TcpAddress tcp_addr(::inet_ntoa(clientaddr.sin_addr), ::ntohs(clientaddr.sin_port));
        utils::setnonblocking(connfd);
        Connection::Pointer conn = Connection::create(loop_, connfd);
        conn->set_status(Connection::eCONNECTING);
        conn->set_address(tcp_addr);
        if (connection_callback_){
            connection_callback_(conn);
        }
    });
    loop->add_event(event_);
}

Listener::~Listener() {
    ::close(listen_socket_);
    delete event_;
}

void Listener::set_new_connection_callback(const NewConnectionCallback &cb) {
    connection_callback_ = cb;
}

void Listener::listen()
{
    int ret;
    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY; //服务器IP地址--允许连接到所有本地地址上
    serveraddr.sin_port = htons(port_); //或者htons(SERV_PORT);

    ret = ::bind(listen_socket_, (sockaddr *) &serveraddr, sizeof(serveraddr));
    if (ret < 0) {
        LOG_DEBUG<<"bind error";
        return;
    }
    ret = ::listen(listen_socket_, LISTENQ);
    if (ret < 0){
        LOG_DEBUG<<"listen error";
        return;
    }
}


}//namespace

