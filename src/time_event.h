#ifndef TIMEEVENT_H
#define TIMEEVENT_H
#include "timer.h"
#include <functional>
#include <memory>
namespace net {

class TimeEvent : public std::enable_shared_from_this<TimeEvent>{
public:
    typedef std::shared_ptr<TimeEvent> Pointer;
    typedef std::function<void(TimeEvent::Pointer)> TimeCallback;

    enum TimeEventType {
        eALWAYS,
        eONCE,
        eNOLONGER,
        eSIZE
    };
public:

    TimeEvent(TimeEventType type = eONCE);
    TimeEvent(const Time& time, TimeEventType type = eONCE);

    static Pointer create(TimeEventType type = eONCE) {
        return std::make_shared<TimeEvent>(type);
    }

    static Pointer create(const Time& time, TimeEventType type = eONCE) {
        return std::make_shared<TimeEvent>(time, type);
    }


    void execute_callback();
    const Time& time() const;
    TimeEventType type() const;
    void cancel();
    void set_type(TimeEventType type);
    void set_time(const Time& time);
    void set_interval(const Time& time);
    void set_time_callback(const TimeCallback& cb);
    void update_next_time();

private:
    Time time_;
    Time interval_;
    TimeCallback time_callback_;
    TimeEventType type_;
};

class TimeEventCompare
{
public:
    bool operator()(const TimeEvent::Pointer& timeEvent1, const TimeEvent::Pointer& timeEvent2) {
        return timeEvent1->time() > timeEvent2->time();
    }
};

}//namespace
#endif // TIMEEVENT_H
