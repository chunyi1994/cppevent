#ifndef POLLER_H
#define POLLER_H
#include <sys/epoll.h>
#include <vector>
#include <map>
#include "socket.h"
namespace net {

class Event;

class Poller {
public:
    Poller();
    ~Poller();
    void add_event(Event *event);
    void update_event(Event *event);
    int wait(std::vector<Event*>* active_events);
    void set_interval(int interval) { interval_ = interval; }

private:
    void ctl(int fd, uint32_t events, int op);
    void add(int fd, uint32_t events);
    void del(int fd, uint32_t events);
    void mod(int fd, uint32_t events);
    epoll_event& get_event(std::size_t i);

    Socket epoll_socket_;
    std::vector<epoll_event> events_;
    std::map<int, Event*> events_map_;
    int interval_;
};

} //namespace

#endif // POLLER_H
