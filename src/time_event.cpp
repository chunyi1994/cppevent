#include "time_event.h"

namespace net {

TimeEvent::TimeEvent(TimeEventType type) :
    time_() ,
    interval_(),
    type_(type),
    executes_times_(0)
{
}

TimeEvent::TimeEvent(const Time &time, TimeEventType type) :
    time_(time) ,
    interval_(),
    type_(type),
    executes_times_(0)
{
}

void TimeEvent::set_time(const Time &time) {
    time_ = time;
}

void TimeEvent::set_interval(const Time &time) {
    interval_ = time;
}

void TimeEvent::set_time_callback(const TimeCallback &cb) {
    time_callback_ = cb;
}

void TimeEvent::update_next_time() {
    time_.now();
    time_.add(interval_);
}

void TimeEvent::execute_callback() {
    if (time_callback_) {
        time_callback_(shared_from_this());
    }
}

const Time &TimeEvent::time() const {
    return time_;
}

void TimeEvent::cancel() {
    type_ = eNOLONGER;
}

void TimeEvent::set_type(TimeEventType type) {
    type_ = type;
}

TimeEvent::TimeEventType TimeEvent::type() const {
    return type_;
}

}//namespace
