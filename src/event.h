#ifndef EVENT_H
#define EVENT_H
#include <functional>
#include <sys/epoll.h>
namespace net {
class EventLoop;
class Event
{
public:
    typedef std::function<void()> EventCallback;
public:
    Event(EventLoop* loop, int fd, uint32_t events);
    void handle_event();
    void set_read_callback(const EventCallback &cb);
    void set_write_callback(const EventCallback &cb);
    void set_error_callback(const EventCallback &cb);
    void set_close_callback(const EventCallback &cb);
    int fd() const;
    void set_fd(int fd);
    uint32_t events() const;
    void set_revents(uint32_t ev);
    void update();
    void enable_reading() { events_ |=   EPOLLIN;  update(); }
    void enable_writing() { events_ |=   EPOLLOUT; update();  }
    void enable_closing() { events_ |=   EPOLLRDHUP; update(); }
    void disable_closing() { events_ &=  !EPOLLRDHUP; update();  }
    void enable_error() { events_ |=   EPOLLERR; update(); }
    void disable_error() { events_ &=  !EPOLLERR; update();  }
    void disable_reading() { events_ &=  !EPOLLIN; update();  }
    void disable_writing() { events_ &=   !EPOLLOUT; update();  }
    void disable_all() {
        events_ &= !EPOLLOUT;
        events_ &= !EPOLLIN;
        events_ &= !EPOLLRDHUP;
        events_ &= !EPOLLERR;
        update();
    }

private:
    EventLoop* loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;

    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback error_callback_;
    EventCallback close_callback_;

};
} // namespace
#endif // EVENT_H
