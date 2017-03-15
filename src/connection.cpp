#include "connection.h"
#include "event_loop.h"
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "logging/logger.h"
namespace net {
Connection::Connection(EventLoop *loop, int fd) :
    connfd_(fd),
    event_(loop, fd,  EPOLLHUP | EPOLLRDHUP ),
    status_(Connection::eCLOSE),
    peer_(),
    buffer_(),
    read_once_(false)
{
    loop->add_event(&event_);
    event_.set_read_callback(std::bind(&Connection::handle_read, this));
    event_.set_close_callback(std::bind(&Connection::handle_close, this));
    event_.set_error_callback(std::bind(&Connection::handle_error, this));
    event_.set_write_callback(std::bind(&Connection::handle_write, this));
}

Connection::~Connection() {
}

void Connection::read_once(Buffer buffer, const Connection::MessageCallback &cb)
{
    buffer_ = buffer;
    message_callback_ = cb;
    read_once_ = true;
    event_.enable_reading();
}

void Connection::read(Buffer buffer, const Connection::MessageCallback &cb)
{
    buffer_ = buffer;
    message_callback_ = cb;
    read_once_ = false;
    event_.enable_reading();
}

//void Connection::set_message_callback(const Connection::MessageCallback &cb) {
//    message_callback_ = cb;
//}

void Connection::set_close_callback(const Connection::CloseCallback &cb) {
    close_callback_ = cb;
}

void Connection::set_error_callback(const Connection::ErrorCallback &cb) {
    error_callback_ = cb;
}

void Connection::set_write_callback(const Connection::MessageCallback &cb) {
    message_callback_ = cb;
}

void Connection::shutdown() {
    handle_close();
}

void Connection::send(const std::string &msg) const {
    send(msg.data(), msg.length());
}

void Connection::send(const char *msg, std::size_t len) const {
    net::send(connfd_.fd(), msg, len);
}

void Connection::handle_close() {
    status_ = eCLOSE;
    if (close_callback_) {
        close_callback_(shared_from_this());
    }
}

void Connection::handle_error() {
    int err = 0;
    socklen_t errlen = sizeof(err);
    std::string errstr = "unknown";
    int errn = errno;
    if (::getsockopt(connfd_.fd(), SOL_SOCKET, SO_ERROR, (void *)&err, &errlen) == 0) {
        errstr = ::strerror(err);
        LOG_DEBUG<<errstr;
    }
    //todo
    if (error_callback_) {
        error_callback_(shared_from_this());
    }
}

void Connection::handle_write() {
    //todo
}

//todo 写的不健壮
void Connection::handle_read() {
    if (!buffer_.data()) {
        LOG_TRACE<<"!buffer_.buf";
        event_.disable_reading();
        return;
    }
    int nread = net::read(connfd_.fd(), buffer_.data(), buffer_.size());
    if (message_callback_) {
        message_callback_(shared_from_this(), buffer_, (std::size_t)nread);
    }
    if (read_once_) {
        buffer_.set_buf(nullptr);
        buffer_.set_size(0);
    }

}

} //namespace

