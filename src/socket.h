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
#include "utils.h"
namespace net {

//raii
class Socket {
public:
    typedef std::shared_ptr<Socket> Pointer;
public:
    Socket(int fd) : fd_(fd) { }

    Socket() : fd_(-1) { }

    ~Socket() ;

    static Pointer create(int fd) { return std::make_shared<Socket>(fd); }

    void set_fd(int fd) ;

    int fd() const { return fd_; }

private:
    //nocopyable
    Socket& operator=(const Socket&) = delete;
    Socket(const Socket&) = delete;
    Socket(Socket&&) = delete;

private:
    int fd_;
};

inline Socket::~Socket() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

inline void Socket::set_fd(int fd) {
    if (fd_ >= 0) {
        ::close(fd_);
    }
    fd_ = fd;
}

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

inline int create_non_blocking_socket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    //assert(socket > 0);
    setnonblocking(sockfd);
    return sockfd;
}

inline std::pair<ErrorCode, int> read_some(int fd, char* buf, int len) {
    int nread = 0;
    int n = 0;
    ErrorCode err(ErrorCode::eOK, "read success");
    while(1){
        n = ::read(fd, buf + nread, len - nread);
        if (n <= 0) {
            if (errno == EAGAIN){
                err.set_code(ErrorCode::eOK);
                err.set_msg("eagain");
            } else {
                err.set_code(ErrorCode::eUNKNOWN);
                err.set_errno(errno);
            }
            break;
        }
        nread += n;
        if (nread == len) {
            break;
        }
    }
    return std::make_pair(err, nread);
}

static std::pair<ErrorCode, int> connect_blocking_helper(
        int sockfd, sockaddr_in& server_addr ) {
    int  status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if( status == -1 ) {
        ErrorCode err(errno);
        LOG_DEBUG<<"connect error:"<<err.msg();
        err.set_code(ErrorCode::eCONNECT_ERR);
        ::close(sockfd);
        return std::make_pair(err, -1);;
    }
    ErrorCode ok(ErrorCode::eOK, "connect succeed");
    return std::make_pair(ok, sockfd);
}

static std::pair<ErrorCode, int> connect_nonblocking_helper(
        int sockfd, sockaddr_in& server_addr ) {
    setnonblocking(sockfd);
    int status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if (status < 0 && errno != EINPROGRESS) {
        ErrorCode conn_err(errno);
        conn_err.set_code(ErrorCode::eCONNECT_ERR);
        LOG_DEBUG<<"Connect error:"<<conn_err.msg();
        std::make_pair(conn_err, -1);
    }
    if (status == 0) {
        std::string errstr = "Connect succeed";
        return std::make_pair(ErrorCode(ErrorCode::eOK, errstr), sockfd);
    }
    ErrorCode wait_err;
    wait_err.set_code(ErrorCode::eCONNECT_IN_PROGRESS);
    wait_err.set_msg("Operation now in progress");
    return std::make_pair(wait_err, sockfd);
}

const int kBLOCKING_CONNECT = 0;
const int kNON_BLOCKING_CONNECT = 1;

static std::pair<ErrorCode, int> connect_helper(
        const std::string &ip,
        int port,
        int blocking = kBLOCKING_CONNECT) {
    int sockfd, status;
    struct sockaddr_in server_addr;
    ::memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = ::inet_aton(ip.c_str(), &server_addr.sin_addr);
    if( status == 0 )  {
        //the server_ip is not valid value
        ErrorCode inet_err(errno);
        inet_err.set_code(ErrorCode::eINET_ERR);
        LOG_DEBUG<<"inet_aton error:" << inet_err.msg();
        return std::make_pair(inet_err, -1);
    }
    //socket出来的值是一次性的
    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if ( sockfd == -1 ) {
        ErrorCode socket_err(errno);
        socket_err.set_code(ErrorCode::eSOCKET_ERR);
        LOG_DEBUG<<"socket error"<<socket_err.msg();
        return std::make_pair(socket_err, -1);
    }
    if (blocking == kNON_BLOCKING_CONNECT) {
        //非阻塞
        return connect_nonblocking_helper(sockfd, server_addr);
    } else {
        //阻塞
        return connect_blocking_helper(sockfd, server_addr);
    }
}


inline int send(int fd, const char* buf, std::size_t len) {
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

inline std::pair<ErrorCode, int>
send_some(int fd, const char* buf, std::size_t len) {
    int nsend = 0;
    int n = 0;
    ErrorCode err(ErrorCode::eOK, "send success");
    while(1){
        n = ::write(fd, buf + nsend, len - nsend);
        if (n <= 0) {
            if (errno == EAGAIN){
                err.set_code(ErrorCode::eOK);
                err.set_msg("eagain");
            } else {
                err.set_code(ErrorCode::eUNKNOWN);
                err.set_errno(errno);
            }
            break;
        }
        nsend += n;
        if (static_cast<std::size_t>(nsend) == len) {
            break;
        }
    }
    return std::make_pair(err, nsend);
}

inline std::pair<ErrorCode, int>
send_some(int fd, const Slice& slice) {
    return send_some(fd,  slice.data(), slice.size());
}

inline int send(const Socket& socket, Buffer buffer) {
    if (!buffer.data()) {
        return -1;
    }
    return send(socket.fd(), buffer.data(), buffer.size());
}

inline int read(const Socket& socket, Buffer buffer) {
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
inline ErrorCode connect(Socket& socket, const std::string &ip, std::size_t port) {
    auto result =  connect_helper(ip, port);
    if (result.first.code() != ErrorCode::eOK) {
        return result.first;
    }
    int fd = result.second;
    socket.set_fd(fd);
    return result.first;
}



}//namespace
#endif // SOCKET

