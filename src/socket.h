#ifndef SOCKET
#define SOCKET
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <memory>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "logging/logger.h"
namespace net {
//raii
class Socket {
public:
    typedef std::shared_ptr<Socket> Pointer;
public:
    Socket(int fd) : fd_(fd) { }

    ~Socket() {
        if (fd_ >= 0) {
            ::close(fd_);
        }
    }

    static Pointer create(int fd) { return std::make_shared<Socket>(fd); }

    void set_fd(int fd) {
        if (fd_ >= 0) {
            ::close(fd_);
        }
        fd_ = fd;
    }

    int fd() const { return fd_; }

private:
    int fd_;
};

//todo 更多的错误处理
static int read(int fd, char* buf, int size) {
    int nread = 0;
    int n = 0;
    while(1){
        n = ::read(fd, buf + nread, size - nread);
        buf[n] = '\0';
        if (n < 0) {
            n = 0;
            if (errno == ECONNRESET){
               LOG_TRACE<<"recv ECONNRESET";
            } else if(errno == EAGAIN){
                LOG_TRACE<<"recv EAGAIN";
            }
            break;
        } else if (n == 0 && errno != EINTR){
            break;
        }
        nread += n;
        if (nread == size) {
            break;
        }
    }
    return nread;
}

static int connect_aux(const std::string &ip, int port) {
    int sockfd, status, save_errno;
    struct sockaddr_in server_addr;
    ::memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = ::inet_aton(ip.c_str(), &server_addr.sin_addr);
    if( status == 0 )  {
        //the server_ip is not valid value
        errno = EINVAL;
        LOG_DEBUG<<"inet_aton error";
        return -1;
    }
    //socket出来的值是一次性的
    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 ){
        LOG_DEBUG<<"socket error";
        return sockfd;
    }

    status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if( status == -1 ) {
        LOG_DEBUG<<"socket error";
        save_errno = errno;
        ::close(sockfd);
        errno = save_errno; //the close may be error
        return status;
    }
    return sockfd;
}

static int send(int fd, const char* buf, std::size_t len) {
    std::size_t wirten = 0;
    int n = 0;
    while (wirten < len){
        n = ::write(fd,  buf + wirten, len - wirten);
        if (n < 0) {
            if (n == -1 && errno != EAGAIN) {
                LOG_TRACE<<"send EAGAIN";
            }
            break;
        }
        wirten += static_cast<std::size_t>(n);
    }
    return wirten;
}

static int send(const Socket& socket, Buffer buffer) {
    if (!buffer.data()) {
        return -1;
    }
    return send(socket.fd(), buffer.data(), buffer.size());
}


static int connect(Socket& socket, const std::string &ip, std::size_t port) {
    int fd =  connect_aux(ip, port);
    if (fd < 0) {
        return fd;
    }
    socket.set_fd(fd);
    return 0;
}

}//namespace
#endif // SOCKET

