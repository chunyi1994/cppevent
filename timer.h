#ifndef TIMER
#define TIMER
#include<sys/time.h>
#include<unistd.h>
#include<sys/time.h>
#include <string>
#include <ctime>
#include <cctype>

namespace cppevent{
typedef __time_t time_t;


class Time{
public:
    Time(const timeval& tv);
    Time(time_t sec = 0, time_t usec = 0);
    bool operator==(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator<(const Time& other) const;  //有bug

    Time& operator+(const Time& other);

    time_t sec() const;  //秒
    time_t usec() const;  //微秒   1秒= 10^6 毫秒
    void setSec(time_t sec);
    void setUSec(time_t usec);
    void addSec(time_t sec);
    void addUSec(time_t usec);
    void add(const Time& other);

    void now();

    struct timeval& timeval();

private:
    struct  timeval tv_;
    struct  timezone tz_;
};


static std::string getTimeStr(){
    std::time_t t;
    std::time ( &t );
    std::string tStr = std::ctime(&t);
    //去掉回车
    tStr.erase(tStr.size() - 1, 1);
    return tStr;
}

}

//printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
//printf("tz_dsttime:%d\n",tz.tz_dsttime);

#endif // TIMER

