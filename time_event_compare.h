#ifndef TIME_EVENT_COMPARE_H
#define TIME_EVENT_COMPARE_H

#include "time_event.h"

namespace cppevent{

class TimeEventCompare
{
public:
    bool operator()(const TimeEventPtr& timeEvent1, const TimeEventPtr timeEvent2)
    {
        if(timeEvent1->getTime() > timeEvent2->getTime())
        {
            return true;
        }
        return false;
    }
};
}
#endif // TIME_EVENT_COMPARE_H

