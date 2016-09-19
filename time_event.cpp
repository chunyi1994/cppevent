#include "time_event.h"

namespace cppevent{

TimeEvent::TimeEvent(TimeEventType type) :
    time_() , interval_(), type_(type)
{

}

TimeEvent::TimeEvent(const Time &time, TimeEventType type) :
    time_(time) , interval_(), type_(type)
{

}

void TimeEvent::setTime(const Time &time)
{
    time_ = time;
}

void TimeEvent::setInterval(const Time &time)
{
    interval_ = time;
}

void TimeEvent::setTimeCallback(const TimeCallback &cb)
{
    timeCallback_ = cb;
}

void TimeEvent::setNextTime()
{
    time_.now();
    time_.add(interval_);
}

void TimeEvent::executeCallback() const
{
    if(timeCallback_)
    {
        timeCallback_();
    }
}

const Time &TimeEvent::getTime() const
{
    return time_;
}

void TimeEvent::cancel()
{
    type_ = TIME_TYPE_NOLONGER;
}

void TimeEvent::setType(TimeEventType type)
{
    type_ = type;
}

TimeEventType TimeEvent::type() const
{
    return type_;
}

}
