#include "eventloop.h"
#include "poller.h"
#include "event.h"
#include <algorithm>
#include "utils.h"
namespace cppevent{

EventLoop::EventLoop():
    quit_(false),
    poller_(std::make_shared<Poller>()),
    taskQueue_(){}

EventLoop::~EventLoop(){}

void EventLoop::loop()
{
    while(!quit_){
        activeEvents_.clear();
        processTimeEvent();
        doTask();//执行投递的一些任务
        poller_->wait(&activeEvents_); //epoll的wait
        std::for_each(activeEvents_.begin(), activeEvents_.end(), [](Event *event){
            event->handleEvent();
        });
    }
}

void EventLoop::addEvent(Event *event)
{
    poller_->addEvent(event);
}

void EventLoop::addTimeEvent(const TimeEventPtr &timeEventPtr)
{
    timeQueue_.push(timeEventPtr);
}

void EventLoop::addTask(const EventLoop::TaskFunc &task)
{
    taskQueue_.push(task);
}

void EventLoop::doTask()
{
    while(!taskQueue_.empty()){
        TaskFunc func = *(taskQueue_.front());
        taskQueue_.pop();
        func();
    }
}

void EventLoop::processTimeEvent()
{
    while(!timeQueue_.empty())
    {
        Time nowTime;
        nowTime.now();
        TimeEventPtr timeEventPtr = timeQueue_.top();
        if(timeEventPtr->getTime() < nowTime)
        {
            timeQueue_.pop();
            timeEventPtr->executeCallback();
            if(timeEventPtr->type() == TIME_TYPE_ALWAYS)
            {
                timeEventPtr->setNextTime();
                timeQueue_.push(timeEventPtr);
            }
        }
        else
        {
            return;
        }

    }
}

void EventLoop::quit()
{
    quit_ = true;
}

TimeEventPtr EventLoop::runAfter(const Time &after, const TimeCallback &cb)
{
    Time atTime;
    atTime.now();
    atTime.add(after);
    TimeEventPtr timeEvent = std::make_shared<TimeEvent>(atTime);
    timeEvent->setTimeCallback(cb);
    addTimeEvent(timeEvent);
    return timeEvent;
}

TimeEventPtr EventLoop::runAt(const Time &atTime, const TimeCallback &cb )
{
        TimeEventPtr timeEvent = std::make_shared<TimeEvent>(atTime);
        timeEvent->setTimeCallback(cb);
        addTimeEvent(timeEvent);
        return timeEvent;
}

TimeEventPtr EventLoop::runEvery(const Time &interval, const TimeCallback &cb)
{
    Time atTime;
    atTime.now();
    TimeEventPtr timeEvent = std::make_shared<TimeEvent>(atTime, TIME_TYPE_ALWAYS);
    timeEvent->setInterval(interval);
    timeEvent->setTimeCallback(cb);
    addTimeEvent(timeEvent);
    return timeEvent;
}

}
