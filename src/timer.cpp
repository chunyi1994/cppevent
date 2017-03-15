#include "timer.h"
#include <assert.h>
namespace net {


Time::Time(const struct timeval &tv) : tv_(tv) {

}

Time::Time(time_t sec, time_t usec) : tv_() {
    tv_.tv_sec = sec;
    tv_.tv_usec = usec;
}


void Time::now() {
    ::gettimeofday(&tv_,&tz_);
}

bool Time::operator==(const Time& other) const {
    return usec() == other.usec() && sec() == other.sec();
}

bool Time::operator>(const Time& other) const {
    if(sec() > other.sec()) {
        return true;
    } else if(sec() < other.sec()) {
        return false;
    }

    if(usec() > other.usec()) {
        return true;
    }

    return false;
}

bool Time::operator<(const Time &other) const {
    if(operator==(other)) {
        return false;
    }
    return !operator>(other);
}

time_t Time::sec() const {
    return tv_.tv_sec;
}

time_t Time::usec() const {
    return tv_.tv_usec;
}

void Time::set_sec(time_t sec) {
    tv_.tv_sec = sec;
}

void Time::set_usec(time_t usec) {
    tv_.tv_usec = usec;
}

void Time::add_usec(time_t usec) {
    //todo 暂定先设置成不进位，以后改正
    assert(usec + tv_.tv_usec < 1000000);
    tv_.tv_usec = usec;
}

void Time::add_sec(time_t sec) {
    //todo 可能会因为sec过大溢出
    tv_.tv_sec += sec;
}

void Time::add(const Time &t) {
    add_usec(t.tv_.tv_usec);
    add_sec(t.tv_.tv_sec);
}

struct timeval& Time::get_timeval() {
    return tv_;
}
}//namespace
