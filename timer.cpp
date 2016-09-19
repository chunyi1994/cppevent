#include "timer.h"
#include <iostream>
namespace cppevent{

Time::Time(const struct timeval &tv) : tv_(tv)
{

}

Time::Time(time_t sec, time_t usec)
{
    tv_.tv_sec = sec;
    tv_.tv_usec = usec;
}


void Time::now()
{
    gettimeofday(&tv_,&tz_);
}

bool Time::operator==(const Time& other) const
{
    return usec() == other.usec() && sec() == other.sec();
}

bool Time::operator>(const Time& other) const
{
    if(sec() > other.sec())
    {
        return true;
    }
    else if(sec() < other.sec())
    {
        return false;
    }

    if(usec() > other.usec()){
        return true;
    }

    return false;
}

bool Time::operator<(const Time &other) const
{
    if(operator==(other))
    {
        return false;
    }
    return !operator>(other);
}

Time &Time::operator+(const Time &other)
{
    add(other);
    return *this;
}

time_t Time::sec() const
{
    return tv_.tv_sec;
}

time_t Time::usec() const
{
    return tv_.tv_usec;
}

void Time::setSec(time_t sec)
{
     tv_.tv_sec = sec;
}

void Time::setUSec(time_t usec)
{
    tv_.tv_usec = usec;
}

void Time::addSec(time_t sec)
{
    tv_.tv_sec += sec;
}

void Time::addUSec(time_t usec)
{
    time_t sum = usec + tv_.tv_usec;
    while(sum > 1000000){
       tv_.tv_sec++;
       sum = sum - 1000000;
    }
    tv_.tv_usec = sum;
}

void Time::add(const Time &other)
{
    addUSec(other.tv_.tv_usec);
    addSec(other.tv_.tv_sec);
}

struct timeval& Time::timeval()
{
    return tv_;
}

}
