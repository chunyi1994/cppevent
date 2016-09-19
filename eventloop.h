#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <memory>
#include <vector>
#include <functional>
#include "thread_safe_queue.h"
namespace cppevent{
class Poller;
class Event;
class EventLoop{
    typedef std::function<void()> TaskFunc;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void addEvent(Event *event);
    void addTask(const TaskFunc& task);  //让EventLoop在下一次循环的时候,取出function执行
    void quit();

private:
    void doTask();
    
    bool quit_;
    std::shared_ptr<Poller> poller_;
    std::vector<Event*> activeEvents_;
    ThreadSafeQueue<TaskFunc> taskQueue_;
};
}
#endif // EVENTLOOP_H
