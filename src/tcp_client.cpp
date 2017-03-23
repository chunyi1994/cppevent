#include "tcp_client.h"
namespace net {

TcpClient::TcpClient(EventLoop *loop) :
    loop_(loop),
    conn_(nullptr),
    connector_(loop),
    buf_(1024 * 2)
{
}

void TcpClient::on_connect(
        const std::string &ip,
        std::size_t port,
        ConnectionCallback cb) {
    connector_.connect(ip, port, [this, cb] (
                       Connection::Pointer conn, ErrorCode errcode) {
        conn_ = conn;
        conn->on_read(message_callback_);

        conn->on_error([this] (Connection::Pointer , ErrorCode) {
            LOG_DEBUG<<"on error";
        });

        conn->on_close([this] (Connection::Pointer) {
            LOG_DEBUG<<"on on_close";
            shutdown();
        });

        if (cb) {
            cb(conn, errcode);
        }
    });
}

void TcpClient::on_read(const MessageCallback &cb) {
    message_callback_ = cb;
}

void TcpClient::on_close(const TcpClient::CloseCallback &cb) {
    close_callback_ = cb;
}

void TcpClient::on_error(const TcpClient::ErrorCallback &cb) {
    error_callback_ = cb;
}

void TcpClient::shutdown() {
    conn_ = nullptr;
}

} //namespace
