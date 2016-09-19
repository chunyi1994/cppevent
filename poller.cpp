#include "poller.h"
#include "event.h"
#include "utils.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <algorithm>
namespace cppevent{

const int MAX_EPOLL_NUM = 20;

Poller::Poller() :
    epSock_(::epoll_create(MAX_EPOLL_NUM)),
    events_(MAX_EPOLL_NUM)
{}

Poller::~Poller(){}

void Poller::addEvent(Event *event){
    assert(event != NULL);
    add(event->fd(), event->events());
    eventsMap_[event->fd()] = event;
}

int Poller::wait(vector<Event*> *activeEvents){
    int maxNumEvents = ::epoll_wait(epSock_.fd(), &*events_.begin(), MAX_EPOLL_NUM, 500); //500应该是毫秒
    std::for_each(events_.begin(), events_.begin() + maxNumEvents,
                  [=](epoll_event event){
        auto ch = eventsMap_.find(event.data.fd);
        assert(ch != eventsMap_.end());
        Event *pevent = ch->second;
        assert(pevent->fd() == event.data.fd);
        pevent->setRevents(event.events); //当前触发的事件
        activeEvents->push_back(pevent);  //所有活动的event被保存在一个vector
    });

    return 0;
}

void Poller::ctl(int fd, uint32_t events, int op){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    ::epoll_ctl(epSock_.fd(), op, fd, &ev);
}

void Poller::add(int fd, uint32_t events){
    ctl(fd, events, EPOLL_CTL_ADD);
}

void Poller::mod(int fd, uint32_t events){
    ctl(fd, events, EPOLL_CTL_MOD);
}

void Poller::del(int fd, uint32_t events){
    ctl(fd, events, EPOLL_CTL_DEL);
}

struct epoll_event& Poller::getEvent(size_t i){
    assert(i < MAX_EPOLL_NUM);
    return events_[i];
}

}
