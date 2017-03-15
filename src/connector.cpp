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
namespace net {





Connector::Connector(EventLoop *loop) :
    loop_(loop)
{
}

Connection::Pointer Connector::connect(const std::string &ip, std::size_t port) {
    int socket  = connect_aux(ip, port);
    if (socket < 0) {
        return nullptr;
    }
    utils::setnonblocking(socket);
    TcpAddress tcpAddr(ip, port);
    Connection::Pointer conn = Connection::create(loop_, socket);
    conn->set_address(tcpAddr);
    conn->set_status(Connection::eCONNECTING);
    return conn;
}



}//namespace
