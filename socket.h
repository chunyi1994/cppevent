#ifndef SOCKET_H
#define SOCKET_H
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <memory>
#include <fcntl.h>
namespace cppevent{
//RAII的Sockst类, 会在析构时关闭fd
class Socket{
public:
    //typedef std::shared_ptr<sockets::InetAddress> AddressPtr;
    Socket(int fd = -1);
    ~Socket();
    void setFd(int fd);
    int fd() const;

private:
    int fd_;
};

//把用于socket设置为非阻塞方式
static void setnonblocking(int sockfd) {
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if (opts < 0) {
        perror("fcntl(sock, GETFL)");
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, opts) < 0) {
        perror("fcntl(sock,SETFL,opts)");
        return;
    }
}

/*
static int createSocket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    //assert(socket > 0);
    return sockfd;
}*/

static int createNonBlockingSocket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    //assert(socket > 0);
    setnonblocking(sockfd);
    return sockfd;
}

/*
static int createNonBlockingSocket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    //assert(sockfd > 0);
    return sockfd;

}*/


}
#endif // SOCKET_H

