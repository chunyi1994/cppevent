#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <memory>
#include <vector>
#include <functional>
#include <queue>

#include "time_event_compare.h"
#include "thread_safe_queue.h"
namespace cppevent{

class Poller;
class Event;

class EventLoop
{
    typedef std::function<void()> TaskFunc;
    typedef std::priority_queue<TimeEventPtr, std::vector<TimeEventPtr>, TimeEventCompare> TimeQueue;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void addEvent(Event *event);
    void quit();

    TimeEventPtr runAt(const Time& atTime, const TimeCallback& cb);
    TimeEventPtr runEvery(const Time& interval, const TimeCallback& cb);
    TimeEventPtr runAfter(const Time& after, const TimeCallback& cb);

    void addTimeEvent(const TimeEventPtr& timeEventPtr);
    void addTask(const TaskFunc& task);  //让EventLoop在下一次循环的时候,取出function执行


private:
    void doTask();
    void processTimeEvent();
    
    bool quit_;
    std::shared_ptr<Poller> poller_;
    std::vector<Event*> activeEvents_;
    ThreadSafeQueue<TaskFunc> taskQueue_;
    TimeQueue timeQueue_;  //二叉堆
};


}
#endif // EVENTLOOP_H
