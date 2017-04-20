#include "event.h"
#include "event_loop.h"
#include "logging/logger.h"
namespace net {


Event::Event(EventLoop *loop, int fd, uint32_t event):
    loop_(loop),
    fd_(fd), events_(event){

}

Event::~Event() {
    loop_->delete_event(this);
}


uint32_t Event::events() const {
    return events_;
}

void Event::set_read_callback(const EventCallback &cb) {
    read_callback_ = cb;
}

void Event::set_write_callback(const EventCallback &cb) {
    write_callback_ = cb;
}

void Event::set_error_callback(const EventCallback &cb) {
    error_callback_ = cb;
}

void Event::set_revents(uint32_t ev){
    revents_ = ev;
}

void Event::update() {
    loop_->update_event(this);
}

void Event::set_close_callback(const EventCallback &cb) {
    close_callback_ = cb;
}

int Event::fd() const {
    return fd_;
}

void Event::set_fd(int fd) {
    fd_ = fd;
}


void Event::handle_event() {
    if(revents_ & EPOLLIN && read_callback_) {
       // LOG_DEBUG<<"EPOLLIN";
        read_callback_();
    }

    if(revents_ & EPOLLOUT && write_callback_) {
        LOG_DEBUG<<"EPOLLOUT";
        write_callback_();
    }

    if(revents_ & EPOLLERR && error_callback_) {
        LOG_DEBUG<<"EPOLLERR";
        error_callback_();
    }

    if(revents_ & EPOLLRDHUP && close_callback_) {
        //当对方close套接字的时候,会调用这里
        LOG_DEBUG<<"EPOLLRDHUP";
        close_callback_();
    }
}

} //namespace
