#include "eventloop.h"
#include "poller.h"
#include "event.h"
#include <algorithm>
#include "utils.h"
namespace cppevent{

EventLoop::EventLoop():quit_(false),poller_(std::make_shared<Poller>()), taskQueue_(){}

EventLoop::~EventLoop(){}

void EventLoop::loop(){
    while(!quit_){
        activeEvents_.clear();
        doTask();//执行投递的一些任务
        poller_->wait(&activeEvents_); //epoll的wait
        std::for_each(activeEvents_.begin(), activeEvents_.end(), [](Event *event){
            event->handleEvent();
        });
    }
}

void EventLoop::addEvent(Event *event){
    poller_->addEvent(event);
}

void EventLoop::addTask(const EventLoop::TaskFunc &task){
    taskQueue_.push(task);
}

void EventLoop::doTask(){
    while(!taskQueue_.empty()){
        TaskFunc func = *(taskQueue_.front());
        taskQueue_.pop();
        func();
    }
}

void EventLoop::quit()
{
    quit_ = true;
}

}
