#ifndef TIMER_H
#define TIMER_H
#include<unistd.h>
#include<sys/time.h>
#include <string>
#include <ctime>
#include <cctype>
namespace net {
class Time
{
public:
    explicit Time(const timeval& tv);
    explicit Time(time_t sec = 0, time_t usec = 0);
    bool operator==(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator<(const Time& other) const;  //有bug

    time_t sec() const;  //秒
    time_t usec() const;  //微秒   1秒= 10^6 毫秒
    void set_sec(time_t sec);
    void set_usec(time_t usec);

    void add_usec(time_t usec);
    void add_sec(time_t sec);
    void add(const Time&);
    void now();

    timeval& get_timeval();

private:
    struct  timeval tv_;
    struct  timezone tz_;
};

}//namespace
#endif // TIMER_H
