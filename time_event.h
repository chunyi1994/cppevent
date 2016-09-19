#ifndef TIMEEVENT_H
#define TIMEEVENT_H
#include <functional>
#include <memory>
#include "timer.h"

namespace cppevent{

typedef std::function<void()> TimeCallback;

enum TimeEventType{
    TIME_TYPE_ALWAYS,
    TIME_TYPE_ONCE,
    TIME_TYPE_NOLONGER
};

class TimeEvent
{
public:
    TimeEvent(TimeEventType type = TIME_TYPE_ONCE);
    TimeEvent(const Time& time, TimeEventType type = TIME_TYPE_ONCE);

    void executeCallback() const;
    const Time& getTime() const;
    TimeEventType type() const;
    void cancel();
    void setType(TimeEventType type);
    void setTime(const Time& time);
    void setInterval(const Time& time);
    void setTimeCallback(const TimeCallback& cb);
    void setNextTime();

private:
    Time time_;
    Time interval_;
    TimeCallback timeCallback_;
    TimeEventType type_;
};

typedef std::shared_ptr<TimeEvent> TimeEventPtr;

}
//---------------设定一个5秒以后执行的定时器

//    Time time1;
//    time1.now();
//    time1.addSec(5); //五秒以后执行
//    TimeEventPtr timeEvent = std::make_shared<TimeEvent>(time1);
//    timeEvent->setTimeCallback([](){
//        log("Test TimeEvent");
//    });
//    loop.addTimeEvent(timeEvent);
//    ......
//    省略
//    ......

#endif // TIMEEVENT_H
