#include "tcp_server.h"
#include "utils.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "event_loop.h"
namespace net {

TcpServer::TcpServer(EventLoop *loop, std::size_t port):
    loop_(loop),
    port_(port),
    listener_(loop, port),
    connections_(),
    buf_(1024*2)
{
    listener_.set_new_connection_callback([this](Connection::Pointer conn) {
        connections_.insert(conn);
        conn->read(Buffer(buf_), [this](const net::Connection::Pointer& c, net::Buffer buffer, std::size_t bytes) {
            if (message_callback_) {
                message_callback_(c, buffer, bytes);
            }
        });
        conn->set_close_callback([this] (net::Connection::Pointer c) {
            if (connection_callback_) {
                connection_callback_(c);
            }
            loop_->add_task([this, c]() {
                auto iter = connections_.find(c);
                assert(iter != connections_.end());
                connections_.erase(iter);
            });
        });

        conn->set_error_callback([this](net::Connection::Pointer c) {
            if (error_callback_) {
                error_callback_(c);
            }
        });

        if (connection_callback_) {
            connection_callback_(conn);
        }
    });
}


TcpServer::~TcpServer() {

}

void TcpServer::start() {
    listener_.listen();
}

void TcpServer::set_message_callback(const MessageCallback &cb) {
    message_callback_ = cb;
}

void TcpServer::set_error_callback(const ErrorCallback &cb) {
    error_callback_ = cb;
}

void TcpServer::shutdown(Connection::Pointer conn) {
    auto iter = connections_.find(conn);
    if (iter != connections_.end()) {
        conn->shutdown();
        connections_.erase(conn);
    }
}

void TcpServer::set_connection_callback(const ConnectionCallback &cb) {
    connection_callback_ = cb;
}


}//namespace
