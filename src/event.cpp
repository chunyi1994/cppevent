#include "event.h"
#include "event_loop.h"
namespace net {


Event::Event(EventLoop *loop, int fd, uint32_t event):
    loop_(loop),
    fd_(fd), events_(event){

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
//    if(revents_ & EPOLLHUP && close_callback_){
//        close_callback_();
//    }

    if(revents_ & EPOLLRDHUP && close_callback_) {
        //当对方close套接字的时候,会调用这里
        close_callback_();
    }

    if(revents_ & EPOLLIN && read_callback_) {
        read_callback_();
    }

    if(revents_ & EPOLLOUT && write_callback_) {
        write_callback_();
    }

    if(revents_ & EPOLLERR && error_callback_) {
        error_callback_();
    }
}

} //namespace
