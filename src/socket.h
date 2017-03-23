#ifndef SOCKET
#define SOCKET
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <memory>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "logging/logger.h"
#include "error_code.h"
#include "slice.h"
namespace net {

//把用于socket设置为非阻塞方式
static void setnonblocking(int sockfd) {
    int opts;
    opts = ::fcntl(sockfd, F_GETFL);
    if (opts < 0) {
        perror("fcntl(sock, GETFL)");
        return;
    }
    opts = opts | O_NONBLOCK;
    if (::fcntl(sockfd, F_SETFL, opts) < 0) {
        ::perror("fcntl(sock,SETFL,opts)");
        return;
    }
}

static int create_non_blocking_socket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    //assert(socket > 0);
    setnonblocking(sockfd);
    return sockfd;
}

//raii
class Socket {
public:
    typedef std::shared_ptr<Socket> Pointer;
public:
    Socket(int fd) : fd_(fd) { }
    Socket() : fd_(-1) { }
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
    std::string errstr;
    while(1){
        n = ::read(fd, buf + nread, size - nread);
        buf[n] = '\0';
        if (n < 0) {
            n = 0;
            if (errno != EAGAIN){
                errstr = ::strerror(errno);
                LOG_DEBUG<<errstr;
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

const int kBLOCKING_CONNECT = 0;
const int kNON_BLOCKING_CONNECT = 1;

static std::pair<ErrorCode, int> connect_helper(
        const std::string &ip,
        int port,
        int blocking = kBLOCKING_CONNECT) {
    int sockfd, status, save_errno;
    struct sockaddr_in server_addr;
    ::memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = ::inet_aton(ip.c_str(), &server_addr.sin_addr);
    std::string errstr;
    if( status == 0 )  {
        //the server_ip is not valid value
        //errno = EINVAL;
        errstr = ::strerror(errno);
        LOG_DEBUG<<"inet_aton error:" << errstr;
        return std::make_pair(ErrorCode(eINET_ERR, errstr), -1);
    }
    //socket出来的值是一次性的
    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 ){
        errstr = ::strerror(errno);
        LOG_DEBUG<<"socket error"<<errstr;
        return std::make_pair(ErrorCode(eSOCKET_ERR, errstr), -1);
    }
    if (blocking == kNON_BLOCKING_CONNECT) {
        setnonblocking(sockfd);
    }
    status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if( status == -1 ) {
        save_errno = errno;
        errstr = ::strerror(errno);
        LOG_DEBUG<<"connect error:"<<errstr;
        ::close(sockfd);
        errno = save_errno; //the close may be error
        return std::make_pair(ErrorCode(eCONNECT_ERR, errstr), -1);;
    }
    return std::make_pair(ErrorCode(eOK, "connect succeed"), sockfd);
}

static int send(int fd, const char* buf, std::size_t len) {
    std::size_t wirten = 0;
    int n = 0;
    while (wirten < len){
        n = ::write(fd,  buf + wirten, len - wirten);
        if (n < 0) {
            if (errno != EAGAIN) {
                std::string errstr = ::strerror(errno);
                LOG_INFO<<errstr;
            }
            break;
        }
        wirten += static_cast<std::size_t>(n);
    }
    return wirten;
}


static int send_some(int fd, const Slice& slice) {
    return ::write(fd,  slice.data(), slice.size());
}

static int send_some(int fd, const char* buf, std::size_t len) {
    return ::write(fd,  buf, len);
}

static int send(const Socket& socket, Buffer buffer) {
    if (!buffer.data()) {
        return -1;
    }
    return send(socket.fd(), buffer.data(), buffer.size());
}

static int read(const Socket& socket, Buffer buffer) {
    if (!buffer.data()) {
        return -1;
    }
    return ::read(socket.fd(), buffer.writeable_data(), buffer.size());
}

static int read_until(
        const Socket& s,        //socket
        std::string& str,         //读取的数据会保存在这里
        std::size_t& pos,       // until的位置
        const std::string& until) {  //这个函数直到读到含有的until字符串才会返回
    int bytes;
    int nread = 0;
    std::vector<char> buf(1024);
    do {
        bytes = read(s, Buffer(buf));
        if (bytes <= 0) {
            //todo
            return bytes;
        }
        std::string recv(buf.data(), bytes);
        nread += bytes;
        str.append(recv);
        pos = str.find(until);
    } while (pos == std::string::npos);
    return nread;
}

static int read_at_least(const Socket& s,  Buffer buffer, std::size_t read_len) {
    assert(read_len <= buffer.size());
    int need_read_len = read_len;
    int bytes;
    do {
        bytes = read(s, buffer);
        if (bytes < 0) {
            //todo
            return bytes;
        }
        need_read_len -= (int)bytes;
    } while (need_read_len > 0);
    return 0;
}

//只有读到read_len的长度才会返回， 数据保存在str里
static int read_at_least(const Socket& s,  std::string& str, std::size_t read_len) {
    if (read_len == 0) {
        return 0;
    }
    int need_read_len = read_len;
    int bytes;
    std::vector<char> buf(1024);
    do {
        bytes = read(s, Buffer(buf));
        if (bytes < 0) {
            //todo
            return bytes;
        }
        std::string recv(buf.data(), bytes);
        str.append(recv);
        need_read_len -= (int)bytes;
    } while (need_read_len > 0);
    return 0;
}


//todo
static ErrorCode connect(Socket& socket, const std::string &ip, std::size_t port) {
    auto result =  connect_helper(ip, port);
    if (result.first.code() != eOK) {
        return result.first;
    }
    int fd = result.second;
    socket.set_fd(fd);
    return result.first;
}

}//namespace
#endif // SOCKET

