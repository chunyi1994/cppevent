#include "tcp_client.h"
#include "event_loop.h"
namespace net {

TcpClient::TcpClient(EventLoop *loop) :
    loop_(loop),
    conns_(),
    connector_(loop),
    buf_(1024 * 2)
{
}

void TcpClient::on_connect(
        const std::string &ip,
        std::size_t port,
        ConnectionCallback cb) {
    on_connect(TcpAddress(ip, port), std::move(cb));
}

void TcpClient::on_connect(const TcpAddress &addr, TcpClient::ConnectionCallback cb) {
    connector_.connect(addr.ip, addr.port, [this, cb] (
                       Connection::Pointer conn, ErrorCode errcode) {
        if (errcode.code() != eOK) {
            if (cb) {
                cb(nullptr, errcode);
            }
            return;
        }
        conns_[conn->address()] = conn;
        //读
        conn->on_read(message_callback_);
        //错误
        conn->on_error([this] (Connection::Pointer c, ErrorCode err) {
            LOG_DEBUG<<"on error:"<<err.msg();
            if (error_callback_) {
                error_callback_(c, err);
            }
            c->shutdown();
        });
        //关闭连接
        conn->on_close([this] (Connection::Pointer c) {
            if (close_callback_) {
                close_callback_(c);
            }
            if (c->status() == Connection::eDELETED) {
                return;
            }
            loop_->add_task([c, this]() {
                auto iter = conns_.find(c->address());
                if (iter != conns_.end()) {
                    conns_.erase(iter);
                }
            });
            c->set_status(Connection::eDELETED);
        });
        //锁住conn
        conn->set_lock(true);

        //connection callback
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

//todo
void TcpClient::shutdown() {
    //auto iter = conns_.find()
}

} //namespace
