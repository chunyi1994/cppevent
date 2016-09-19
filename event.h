#ifndef EVENT_H
#define EVENT_H
#include <functional>
#include <sys/epoll.h>

namespace cppevent{
typedef std::function<void()> EventCallback;
class Event{
public:
    Event(int fd, uint32_t events);
    void handleEvent();
    void setReadCallback(const EventCallback &cb);
    void setWriteCallback(const EventCallback &cb);
    void setErrorCallback(const EventCallback &cb);
    void setCloseCallback(const EventCallback &cb);
    int fd() const;
    void setFd(int fd);
    uint32_t events() const;
    void setRevents(uint32_t ev);

private:
    int fd_;
    uint32_t events_;
    uint32_t revents_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

};
}
#endif // EVENT_H
