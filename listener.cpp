#include "listener.h"
#include "utils.h"
#include "event.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "eventloop.h"
#include <stdio.h>
#include <errno.h>
using std::bind;
namespace cppevent{
const int LISTENQ = 10;
Listener::Listener(EventLoop* loop, size_t port) :
    port_(port) ,
    listenSocket_(createNonBlockingSocket()),
    event_(new Event(listenSocket_.fd(), EPOLLIN | EPOLLET ))
{
    event_->setReadCallback(bind(&Listener::handleRead, this));
    loop->addEvent(event_);
}

Listener::~Listener()
{
    delete event_;
}

void Listener::setNewConnectionCallback(const ListenerConnectionCallback &cb)
{
    connectionCallback_ = cb;
}

void Listener::listen()
{
    int ret;
    sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY; //服务器IP地址--允许连接到所有本地地址上
    serveraddr.sin_port = htons(port_); //或者htons(SERV_PORT);

    ret = ::bind(listenSocket_.fd(), (sockaddr *) &serveraddr, sizeof(serveraddr));
    if(ret < 0){
        perror("bind err:");
    }
    ret = ::listen(listenSocket_.fd(), LISTENQ);
    if(ret < 0){
        perror("listen err:");
    }
}

void Listener::handleRead()
{
    sockaddr_in clientaddr;
    size_t clilen = sizeof(clientaddr);

    memset(&clientaddr, 0, sizeof(struct sockaddr_in));
    int connfd = accept(listenSocket_.fd(), (sockaddr *) &clientaddr, &clilen);
    if(connfd < 0){
        perror("accept err:");
    }
    if(connectionCallback_){
        connectionCallback_(connfd, &clientaddr, clilen);
    }
}


}
