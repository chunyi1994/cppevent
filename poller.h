#ifndef POLLER_H
#define POLLER_H
#include <sys/epoll.h>
#include <vector>
#include <map>

#include "socket.h"

using std::vector;
using std::map;
namespace cppevent{

class Event;

class Poller
{
public:
    Poller();
    ~Poller();
    void addEvent(Event *event);
    int wait(vector<Event*> *activeEvents);

private:
    void ctl(int fd, uint32_t events, int op);
    void add(int fd, uint32_t events);
    void del(int fd, uint32_t events);
    void mod(int fd, uint32_t events);
    epoll_event& getEvent(size_t i);

    Socket epSock_;
    vector<epoll_event> events_;
    map<int, Event*> eventsMap_;
};
}
#endif // POLLER_H
