#include "connector.h"
#include <functional>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include <string.h>

#include "utils.h"
#include "logging/logger.h"
#include "socket.h"
#include "event_loop.h"
namespace net {

Connector::Connector(EventLoop *loop) :
    loop_(loop),
    timeout_(8)
{
}

void Connector::connect(  std::string ip,
                          std::size_t port,
                          Connector::ConnectionCallback cb) {
    connect(TcpAddress(ip, port), std::move(cb));
}

void Connector::connect(const TcpAddress &addr,
                        Connector::ConnectionCallback cb) {
    std::pair<ErrorCode, int> result  =
            connect_helper(addr.ip, addr.port, kNON_BLOCKING_CONNECT);
    int sockfd = result.second;
    int code = result.first.code();
    switch (code) {
    case ErrorCode::eOK:{
        //情况一，可能是连接本地，直接连接成功
        auto conn = new_connection(sockfd, addr);
        ErrorCode err(ErrorCode::eOK, "connect now success");
        cb(conn, err);
        break;
    }
    case ErrorCode::eCONNECT_IN_PROGRESS:
        //情况二，connect还处在三次握手没完成
        process_in_progress_connect(sockfd, addr, std::move(cb));
        break;

    default:
        //情况三 connect连接失败
        cb(nullptr, result.first);
        break;
    }//switch
}

Connection::Pointer Connector::new_connection(int fd, const TcpAddress &addr) {
    Connection::Pointer conn = Connection::create(loop_, fd);
    conn->set_address(addr);
    conn->set_status(Connection::eCONNECTING);
    return conn;
}

//处理需要等待的connect请求
void Connector::process_in_progress_connect(int fd,
                                     TcpAddress addr,
                                     ConnectionCallback cb) {
    auto conn = new_connection(fd, addr);
    conns_[conn] = 0;
    //当conn有写信号的时候，说明有了结果，可能是失败也可能成功，
    //具体看有没有error信号
    conn->on_write([this] (Connection::Pointer conn) {
        if (conns_[conn] == 0) {
            conns_[conn] = 1;
        }
    });
    //当conn有error信号，说明connect失败
    conn->on_error([this] (Connection::Pointer conn, ErrorCode code) {
        LOG_DEBUG<<"connect error:"<<code.msg();
        conns_[conn] = -1;
    });
    check_connect_state(conn, std::move(cb));
}

void Connector::remove(Connection::Pointer conn) {
    auto iter = conns_.find(conn);
    if (iter != conns_.end()) {
        conns_.erase(iter);
    }
}

void Connector::check_connect_state(Connection::WeakPointer connweak,
                                    ConnectionCallback cb) {
    loop_->run_every(Time(1),
                     [this, cb, connweak] (TimeEvent::Pointer time_event) {
        Connection::Pointer conn = connweak.lock();
        assert(conn);
        if (time_event->execute_times() > timeout_) {
            remove(conn);
            ErrorCode timeout(ErrorCode::eCONNECT_ERR, "connect timedout.");
            cb(nullptr, timeout);
            time_event->cancel();
            return;
        }
        auto iter = conns_.find(conn);
        assert(iter != conns_.end());
        switch (iter->second) {
        case 0:
            break;

        case -1:
            remove(conn);
            cb(nullptr, ErrorCode(
                   ErrorCode::eCONNECT_ERR, "connect failed"));
            time_event->cancel();
            break;

        case 1:
            conn->on_write([](Connection::Pointer) {});
            conn->on_error([](Connection::Pointer, ErrorCode) {});
            conn->disable_writing();
            conn->disable_error();
            cb(conn,  ErrorCode(ErrorCode::eOK, "connect success"));
            remove(conn);
            time_event->cancel();
            break;
        default:
            assert(0);
            break;
        }
    });
}

Connection::Pointer Connector::connect(const std::string &ip,
                                       std::size_t port) {
    std::pair<ErrorCode, int> result
            = connect_helper(ip, port, kBLOCKING_CONNECT);
    if (result.first.code() != ErrorCode::eOK) {
        return nullptr;
    }
    setnonblocking(result.second);
    TcpAddress tcpAddr(ip, port);
    Connection::Pointer conn = Connection::create(loop_, result.second);
    conn->set_address(tcpAddr);
    conn->set_status(Connection::eCONNECTING);
    return conn;
}

Connection::Pointer Connector::connect(const TcpAddress &addr) {
    return connect(addr.ip, addr.port);
}



}//namespace
