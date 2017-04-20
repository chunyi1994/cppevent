#include "poller.h"
#include <assert.h>
#include "event.h"
#include <algorithm>
namespace net {

const int kMAX_EPOLL_NUM = 65535;


Poller::Poller()  :
    epoll_socket_(::epoll_create(kMAX_EPOLL_NUM)),
    events_(kMAX_EPOLL_NUM),
    events_map_()
{

}

Poller::~Poller()
{

}

void Poller::add_event(Event *event)
{
    assert(event);
    add(event->fd(), event->events());
    events_map_[event->fd()] = event;
}

void Poller::delete_event(Event *event) {
    assert(event);
    auto iter = events_map_.find(event->fd());
    assert(iter != events_map_.end());
    events_map_.erase(iter);
    del(event->fd(), event->events());
}

void Poller::update_event(Event *event)
{
    assert(event);
    mod(event->fd(), event->events());
    events_map_[event->fd()] = event;
}

int Poller::wait(std::vector<Event *> *active_events, int interval) {
    int events_num = ::epoll_wait(epoll_socket_.fd(),
                                    &*events_.begin(),
                                  kMAX_EPOLL_NUM, interval);
    if (events_num < 0) {
        return events_num;
    }

    std::for_each(events_.begin(), events_.begin() + events_num, [=](epoll_event event) {
        auto ch = events_map_.find(event.data.fd);
        assert(ch != events_map_.end());
        Event *pevent = ch->second;
        assert(pevent->fd() == event.data.fd);
        pevent->set_revents(event.events); //当前触发的事件
        active_events->push_back(pevent);  //所有活动的event被保存在一个vector
    });
    return 0;
}

void Poller::ctl(int fd, uint32_t events, int op) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    ::epoll_ctl(epoll_socket_.fd(), op, fd, &ev);
}
void Poller::add(int fd, uint32_t events) {
    ctl(fd, events, EPOLL_CTL_ADD);
}

void Poller::mod(int fd, uint32_t events) {
    ctl(fd, events, EPOLL_CTL_MOD);
}

void Poller::del(int fd, uint32_t events) {
    ctl(fd, events, EPOLL_CTL_DEL);
}

struct epoll_event& Poller::get_event(std::size_t i) {
    assert(i < kMAX_EPOLL_NUM);
    return events_[i];
}

} //namespace
