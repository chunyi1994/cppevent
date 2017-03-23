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
    //buffer_(),
    // read_once_(false),
    create_time_(),
    last_active_time_(),
    send_buffer_(),
    recv_buffer_(),
    context_()
{
    create_time_.now();
    loop->add_event(&event_);
    event_.set_read_callback(std::bind(&Connection::handle_read, this));
    event_.set_close_callback(std::bind(&Connection::handle_close, this));
    event_.set_error_callback(std::bind(&Connection::handle_error, this));
    event_.set_write_callback(std::bind(&Connection::handle_write, this));
}

Connection::~Connection() {
    LOG_TRACE<<"~Connection";
}

//void Connection::read_once(Buffer buffer, const Connection::MessageCallback &cb) {
//    buffer_ = buffer;
//    message_callback_ = cb;
//    read_once_ = true;
//    event_.enable_reading();
//}

void Connection::on_read(const Connection::MessageCallback &cb) {
    //buffer_ = buffer;
    message_callback_ = cb;
    //read_once_ = false;
    event_.enable_reading();
}

//void Connection::set_message_callback(const Connection::MessageCallback &cb) {
//    message_callback_ = cb;
//}

void Connection::on_close(const Connection::CloseCallback &cb) {
    close_callback_ = cb;
}

void Connection::on_error(const Connection::ErrorCallback &cb) {
    error_callback_ = cb;
}

void Connection::on_write(const Connection::MessageCallback &cb) {
    message_callback_ = cb;
}

void Connection::shutdown() {
    handle_close();
}

void Connection::send(const std::string &msg) const {
    send(msg.data(), msg.length());
}

void Connection::send(const char *msg, std::size_t len) const {
    if (!send_buffer_.empty()) {
        send_buffer_.append(msg, len);
        return;
    }
    int bytes = net::send_some(connfd_.fd(), msg, len);
    if (bytes < 0) {
        if (errno != EAGAIN) {
            std::string errstr = ::strerror(errno);
            LOG_DEBUG<<errstr;
            return;
        }
        bytes = 0;
    }
    if (static_cast<std::size_t>(bytes) == len) {
        return;
    }
    send_buffer_.append(msg + bytes, len - static_cast<std::size_t>(bytes));
    event_.enable_writing();
}

time_t Connection::alive_time() const {
    Time t;
    t.now();
    return t.sec() - create_time_.sec();
}

void Connection::handle_close() {
    status_ = eCLOSE;
    event_.disable_all();
    if (close_callback_) {
        close_callback_(shared_from_this());
    }
}

void Connection::handle_error() {
    std::string errstr = ::strerror(errno);
    event_.disable_all();
    ErrorCode error_code(eUNKNOWN, errstr);
    status_ = eERROR;
    if (error_callback_) {
        error_callback_(shared_from_this(), error_code);
    }
}

void Connection::handle_write() {
    if (send_buffer_.empty()) {
        event_.disable_writing();
        return;
    }
    Slice slice = send_buffer_.slice(send_buffer_.size());
    int bytes = net::send_some(connfd_.fd(), slice);
    if (bytes < 0) {
        if (errno != EAGAIN) {
            std::string errstr = ::strerror(errno);
            LOG_DEBUG<<errstr;
            return;
        }
        bytes = 0;
    }
    send_buffer_.erase(bytes);
}

void Connection::handle_read() {
    last_active_time_.now();
    char buf[1024];
    int nread = net::read(connfd_.fd(), buf, sizeof(buf));
    if (nread != 0) {
        if (message_callback_) {
            recv_buffer_.append(buf, static_cast<std::size_t>(nread));
            message_callback_(shared_from_this());
        }
    }
}
//void Connection::handle_read() {
//    if (!buffer_.writeable_data()) {
//        LOG_TRACE<<"!buffer_.buf";
//        event_.disable_reading();
//        return;
//    }
//    last_active_time_.now();
//    int nread = net::read(connfd_.fd(), buffer_.writeable_data(), buffer_.size());
//    if (nread != 0) {
//        if (message_callback_) {
//            message_callback_(shared_from_this(), buffer_, (std::size_t)nread);
//        }
//    } else {
//        LOG_DEBUG<<"nread == 0";
//    }

//    if (read_once_) {
//        buffer_.set_write_buf(nullptr);
//        buffer_.set_size(0);
//    }
//}

// socklen_t errlen = sizeof(err);
//int errn = errno;
//    if (::getsockopt(connfd_.fd(), SOL_SOCKET, SO_ERROR, (void *)&err, &errlen) == 0) {
//        errstr = ::strerror(err);
//        LOG_DEBUG<<errstr;
//    }


} //namespace

