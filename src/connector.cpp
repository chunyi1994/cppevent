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
    loop_(loop)
{
}

void Connector::connect(
        std::string ip,
        std::size_t port,
        Connector::ConnectionCallback cb,
        time_t timeout) {
    loop_->run_every(Time(timeout, 0), [this, ip, port, cb](TimeEvent::Pointer event) {
        auto result  = connect_helper(ip, port, kBLOCKING_CONNECT);
        if (result.first.code() == eOK) {
            int sockfd = result.second;
            setnonblocking(sockfd);
            Connection::Pointer conn = Connection::create(loop_, sockfd);
            conn->set_address(TcpAddress(ip, port));
            conn->set_status(Connection::eCONNECTING);
            ErrorCode code(eOK, "connect success");
            cb(conn, code);
        } else {
            if (event->execute_times() < 3) {
                return;
            }
            cb(nullptr, result.first);
        }
        event->cancel();
    });
    return;
}

Connection::Pointer Connector::connect(const std::string &ip, std::size_t port) {
     auto result  = connect_helper(ip, port, kBLOCKING_CONNECT);

    if (result.first.code() != eOK) {
        return nullptr;
    }
    setnonblocking(result.second);
    TcpAddress tcpAddr(ip, port);
    Connection::Pointer conn = Connection::create(loop_, result.second);
    conn->set_address(tcpAddr);
    conn->set_status(Connection::eCONNECTING);
    return conn;
}



}//namespace
