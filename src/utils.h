#ifndef UTILS
#define UTILS
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <map>
#include <functional>
#include <string>
#include <sstream>
#include "connection.h"
#include "coroutine/coroutine.h"
namespace utils {

//协程函数：至少读n个数据再返回
//net::Connection::Pointer conn,
//net::Buffer buffer,
//std::size_t n,
//coroutine::Coroutine* co
static std::size_t read_at_least(net::Connection::Pointer conn,
                                 net::Buffer buffer,
                                 std::size_t n,
                                 coroutine::Coroutine* co) {
    bool lock = false;
    //由于协程的一个特性，这里用指针
    //具体原因我猜想是，当协程调用yield的时候，会保存现场，nread保存为0
    //之后lambda修改了nread，但是当协程回到原处的时候，会把之前保存的nread恢复
    //解决办法是采用指针，指向栈上的空间。
    std::size_t* nread = new std::size_t(0);
    assert(n <= buffer.size());
    while ( *nread < n
            && conn->status() == net::Connection::eCONNECTING) {
        if (lock) {
            co->yield();
        }
        net::Buffer buf(buffer.data() + *nread, buffer.size() - *nread);
        conn->read_once(buf,  [nread](const net::Connection::Pointer&,
                        net::Buffer,
                        std::size_t bytes) {
            *nread += bytes;
        });
        lock = true;
    }
    std::size_t result = *nread;
    delete nread;
    return result;
}


//协程函数：至少读n个数据再返回
//net::Connection::Pointer conn,
//net::Buffer buffer,
//std::size_t n,
//coroutine::Coroutine* co
static std::size_t read_once(net::Connection::Pointer conn,
                             net::Buffer buffer,
                             coroutine::Coroutine* co) {
    bool *lock = new bool(true);
    std::size_t* nread = new std::size_t(0);
    conn->read_once(buffer,  [lock, nread](const net::Connection::Pointer&,
                    net::Buffer,
                    std::size_t bytes) {
        *nread += bytes;
        *lock = false;
    });
    while (*lock && conn->status() == net::Connection::eCONNECTING) {
        co->yield();
    }
    std::size_t result = *nread;
    delete nread;
    delete lock;
    return result;
}

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

static int create_non_blocking_socket(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    //assert(socket > 0);
    setnonblocking(sockfd);
    return sockfd;
}

template<class T>
static std::string to_string(T value) {
    std::stringstream ss;
    ss<<value;
    return ss.str();
}

template <class T>
static int to_int(const T& value) {
    int result = 0;
    std::stringstream ss;
    ss << value;
    ss >> result;
    return result;
}

static bool trim(std::string& str) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase( str.find_last_not_of(" ") + 1);
    return true;
}

static bool trim_quo(std::string& str) {
    if (str.empty()) {
        return false;
    }
    str.erase(0, str.find_first_not_of("\""));
    str.erase(str.find_last_not_of("\"") + 1);
    return true;
}

static std::vector<std::string> split(const std::string& str, char c) {
    std::string line;
    std::stringstream ss(str);
    std::vector<std::string> result;
    while (std::getline(ss, line, c)) {
        result.push_back(line);
    }
    return result;
}
//todo 可能有问题
static std::vector<std::string> split(const std::string& str, const std::string& s) { //s是分隔符
    std::vector<std::string> result;
    std::size_t left = 0;
    std::size_t right = 0;
    while ( (right = str.find(s, left))  != std::string::npos)  {
        result.push_back(str.substr(left, right));
        left = right + s.length();
    }
    if (left < str.length()) {
        result.push_back(str.substr(left, str.size() - left));
    }
    return result;
}

static std::vector<std::string> split2(const std::string& str, const std::string& s) { //s是分隔符
    std::vector<std::string> result;
    std::size_t left = 0;
    std::size_t right = 0;
    while ( (right = str.find(s, left))  != std::string::npos)  {
        std::string m = str.substr(left, right);
        if (!m.empty()) {
            result.push_back(m);
        }
        left = right + s.length();
    }
    if (left < str.length()) {
        result.push_back(str.substr(left, str.size() - left));
    }
    return result;
}

} // namespace
#endif // UTILS

