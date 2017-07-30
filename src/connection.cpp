#include "connection.h"
#include "event_loop.h"
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "logging/logger.h"

namespace net {

int Connection::g_connections = 0;


Connection::Connection(EventLoop *loop, int fd) :
    name_("conn"),
    connfd_(fd),
    event_(loop, fd,  EPOLLHUP | EPOLLRDHUP ),
    status_(Connection::eCLOSED),
    peer_(),
    create_time_(),
    last_active_time_(),
    buf_(1024 * 3),
    send_buffer_(),
    recv_buffer_(),
    context_(),
    callback_lock_(false)
{
    create_time_.now();
    loop->add_event(&event_);
    event_.set_read_callback(std::bind(&Connection::handle_read, this));
    event_.set_close_callback(std::bind(&Connection::handle_close, this));
    event_.set_error_callback(std::bind(&Connection::handle_error, this));
    event_.set_write_callback(std::bind(&Connection::handle_write, this));
}


//todo 被锁住了应该加提示
void Connection::on_read(const Connection::MessageCallback &cb) {
    if (callback_lock_) {
        LOG_DEBUG<<"callback被锁，无法修改on_read callback";
        return;
    }
    read_callback_ = cb;
    enable_reading();
}

void Connection::enable_writing() {
    event_.enable_writing();
}

void Connection::enable_reading() {
    event_.enable_reading();
}

void Connection::disable_writing() {
    event_.disable_writing();
}

void Connection::disable_reading() {
    event_.disable_reading();
}

void Connection::disable_error() {
    event_.disable_error();
}

void Connection::disable_all() {
    event_.disable_all();
}

void Connection::on_close(const Connection::CloseCallback &cb) {
    if (callback_lock_) {
        LOG_DEBUG<<"callback被锁，无法修改on_close callback";
        return;
    }
    close_callback_ = cb;
}

void Connection::on_error(const Connection::ErrorCallback &cb) {
    if (callback_lock_) {
        LOG_DEBUG<<"callback被锁，无法修改on_error callback";
        return;
    }
    error_callback_ = cb;
}

void Connection::on_write(const Connection::MessageCallback &cb) {
    if (callback_lock_) {
        LOG_DEBUG<<"callback被锁，无法修改on_write callback";
        return;
    }
    write_callback_ = cb;
    enable_writing();
}

void Connection::send(const std::string &msg) {
    send(msg.data(), msg.size());
}

void Connection::send(const std::vector<char> &msg) {
    send(msg.data(), msg.size());
}

void Connection::send(const char *msg, std::size_t len) {
    if (status() != eCONNECTING) {
        LOG_DEBUG<<"发送失败";
        shutdown();
        return;
    }
    // 如果send buffer已经有数据没有发送
    // 为了避免数据顺序错乱
    // 应该把数据加在buffer的尾部
    if (!send_buffer_.empty()) {
        send_buffer_.append(msg, len);
        return;
    }
    //先试着发送一点数据，然后把没发送的保存下来
    //留着下次发，并打开write监视，当数据发送完成时候，关闭write监视
    std::pair<ErrorCode, int> result
            = net::send_some(connfd_.fd(), msg, len);
    ErrorCode& error = result.first;
    if (error.code() != ErrorCode::eOK) {
        LOG_DEBUG<<"error:"<<error.msg();
        shutdown();
        return;
    }
    std::size_t bytes = static_cast<std::size_t>(result.second);
    if (bytes == len) {
        //证明全部发送完毕
        return;
    }
    send_buffer_.append(msg + bytes, len - bytes);
    enable_writing();
}

time_t Connection::alive_time() const {
    Time t;
    t.now();
    return t.sec() - create_time_.sec();
}

//可以重复调用，确保只调用callback一次
void Connection::shutdown() {
    Connection::Pointer myself = shared_from_this();
    if (status_ == eCLOSED) {
        return;
    }
    status_ = eCLOSED;
    disable_all();
    if (close_callback_) {
        close_callback_(myself);
    }
}

void Connection::close() {
    if (status_ == eCONNECTING) {
        status_ = eCLOSING;
    }
    if (send_buffer_.empty()) {
        shutdown();
    }
}

void Connection::handle_close() {
    //LOG_DEBUG<<"EPOLLRDHUP, fd:"<<fd()<<", name:"<<name();
    if (!recv_buffer_.empty() && read_callback_) {
        read_callback_(shared_from_this());
    }
    shutdown();
    return;
}

void Connection::handle_error() {
    //LOG_DEBUG<<"EPOLLERR, fd:"<<fd()<<", name:"<<name();
    ErrorCode error_code(errno);
    disable_all();
    if (status_ == eCONNECTING) {
        status_ = eERROR;
    }
    if (error_callback_) {
        error_callback_(shared_from_this(), error_code);
    }
}

void Connection::handle_write() {
    //LOG_DEBUG<<"EPOLLOUT, fd:"<<fd()<<", name:"<<name();
    if (send_buffer_.empty()) {
        if (write_callback_) {
            write_callback_(shared_from_this());
        }
        disable_writing();
        if (status_ == eCLOSING) {
            shutdown();
        }
        return;
    }
    Slice slice = send_buffer_.slice(send_buffer_.size());
    std::pair<ErrorCode, int> result
            = net::send_some(connfd_.fd(), slice);
    ErrorCode& error = result.first;
    if (error.code() != ErrorCode::eOK) {
        LOG_DEBUG<<"error:"<<error.msg();
        shutdown();
        return;
    }
    int sendbytes = result.second;
    assert(sendbytes > 0);
    send_buffer_.erase(sendbytes);
    if (write_callback_) {
        write_callback_(shared_from_this());
    }
}

void Connection::handle_read() {
    //LOG_DEBUG<<"EPOLLIN, fd:"<<fd()<<", name:"<<name();
    last_active_time_.now();
    std::pair<ErrorCode, int> result =
            net::read_some(connfd_.fd(), buf_.data(), buf_.size());
    int nread = result.second;
    //ErrorCode err = result.first;
    if (nread < 0) {
        return;
    }
    if (read_callback_) {
        recv_buffer_.append(buf_.data(), static_cast<std::size_t>(nread));
        read_callback_(shared_from_this());
    }
}

} //namespace

