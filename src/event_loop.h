#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include "coroutine/coroutine.h"
#include "time_event.h"
namespace net {

class Event;
class Poller;

class EventLoop
{
public:
    typedef std::function<void()> TaskFunction;
    typedef std::priority_queue<
          TimeEvent::Pointer, std::vector< TimeEvent::Pointer>, TimeEventCompare
        > TimeQueue;

public:
    EventLoop();
    ~EventLoop();
    void loop();
    void add_event(Event *event);
    void add_time_event(const TimeEvent::Pointer&);
    void quit();
    void add_task(const TaskFunction& task);
    void update_event(Event* event);
    void run_every(const Time& interval, TimeEvent::TimeCallback cb);
    void run_at(const Time& time, const TimeEvent::TimeCallback& cb);
    void run_after(const Time& time, const TimeEvent::TimeCallback& cb);
    coroutine::Coroutine::Pointer start_coroutine(const coroutine::Schedule::Function &func);
private:
    void execute_task();
    void execute_time_task();
    void execute_coroutines();
private:
    bool quit_;
    std::vector<Event*> active_events_;
    std::shared_ptr<Poller> poller_;
    std::queue<TaskFunction> task_queue_;
    coroutine::Schedule schedule_;
    TimeQueue time_queue_;
};

} // namespace

#endif // EVENTLOOP_H
