#include "event_loop.h"
#include <algorithm>
#include "event.h"
#include "poller.h"
#include "linux_signal.h"
#include "logging/logger.h"
namespace net {
EventLoop::EventLoop() :
    quit_(false),
    poller_(std::make_shared<Poller>()),
    task_queue_(),
    time_queue_()
{
}


EventLoop::~EventLoop()
{
}

const int kINTERVAL = 500;
void EventLoop::loop() {
    //sigpipe
    Signal::signal(SIGPIPE, []() { LOG_DEBUG<<"sigpipe."; });

    while (!quit_) {
        //add_task 的任务
        execute_task();
        //time event的任务
        execute_time_task();
        //执行几个协程任务
        execute_coroutines();
        active_events_.clear();
        //epoll的wait
        poller_->wait(&active_events_, kINTERVAL);
        std::for_each(active_events_.begin(), active_events_.end(), [](Event *event){
            event->handle_event();
        });
    }
}

std::size_t EventLoop::epoll_event_nums() const {
    return poller_->epoll_event_nums();
}

void EventLoop::add_event(Event *event) {
    poller_->add_event(event);
}

void EventLoop::delete_event(Event *event) {
    poller_->delete_event(event);
}

void EventLoop::add_time_event(const TimeEvent::Pointer & event) {
    time_queue_.push(event);
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::add_task(const EventLoop::TaskFunction &task) {
    task_queue_.push(task);
}

void EventLoop::update_event(Event *event) {
    poller_->update_event(event);
}

void EventLoop::run_every(const Time& interval, TimeEvent::TimeCallback cb) {
    TimeEvent::Pointer event = TimeEvent::create(TimeEvent::eALWAYS);
    event->set_interval(interval);
    event->set_time_callback(std::move(cb));
    add_time_event(event);
}

void EventLoop::run_at(const Time& time, const TimeEvent::TimeCallback& cb) {
    TimeEvent::Pointer event = TimeEvent::create(TimeEvent::eONCE);
    event->set_time(time);
    event->set_time_callback(cb);
    add_time_event(event);
}

void EventLoop::run_after(const Time &time, const TimeEvent::TimeCallback &cb) {
    TimeEvent::Pointer event = TimeEvent::create(TimeEvent::eONCE);
    Time t;
    t.now();
    t.add(time);
    event->set_time(t);
    event->set_time_callback(cb);
    add_time_event(event);
}

coroutine::Coroutine::Pointer EventLoop::start_coroutine(const coroutine::Schedule::Function &func) {
    return schedule_.new_coroutine(func);
}

void EventLoop::execute_task() {
    while (!task_queue_.empty()) {
        auto task = task_queue_.front();
        task_queue_.pop();
        task();
    }
}

void EventLoop::execute_time_task() {
    while(!time_queue_.empty()) {
        Time time_now;
        time_now.now();
        TimeEvent::Pointer event = time_queue_.top();
        if (event->time() > time_now) {
            return;
        }
        time_queue_.pop();
        event->set_execute_times(event->execute_times() + 1);
        event->execute_callback();
        if (event->type() == TimeEvent::eALWAYS) {
            event->update_next_time();
            time_queue_.push(event);
        }
    }
}

void EventLoop::execute_coroutines() {
    if (!schedule_.empty()) {
        schedule_.resume_all();
    }
}
} //namespace
