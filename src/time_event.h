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

    static Pointer create(TimeEventType type = eONCE) {
        return std::make_shared<TimeEvent>(type);
    }

    static Pointer create(const Time& time, TimeEventType type = eONCE) {
        return std::make_shared<TimeEvent>(time, type);
    }
public:

    explicit TimeEvent(TimeEventType type = eONCE);
    explicit TimeEvent(const Time& time, TimeEventType type = eONCE);

    void execute_callback();

    const Time& time() const;

    TimeEventType type() const;

    void cancel();

    void set_type(TimeEventType type);

    void set_time(const Time& time);

    void set_interval(const Time& time);

    void set_time_callback(const TimeCallback& cb);

    void update_next_time();

    void set_execute_times(int times) { executes_times_ = times; }

    int execute_times  () const {  return executes_times_; }

private:
    TimeEvent& operator=(const TimeEvent&) = delete;

    TimeEvent(const TimeEvent&) = delete;

    TimeEvent(TimeEvent&&) = delete;

private:
    Time time_;

    Time interval_;

    TimeCallback time_callback_;

    TimeEventType type_;

    int executes_times_;
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
