#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <iostream>
#include <cctype>
#include <ctime>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
namespace cppevent{

static bool trim(std::string& str)
{
    if (str.empty())
    {
        return false;
    }

    str.erase(0, str.find_first_not_of(" "));
    str.erase( str.find_last_not_of(" ") + 1);
    return true;
}

static bool beginWith(const std::string& src, const std::string& dest)
{
    if(dest.length() > src.length())
    {
        return false;
    }

    if(src.substr(0, dest.length()) == dest){
        return true;
    }

    return false;
}

static long getTimeOfDaySec(){
    struct  timeval    tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

static long getTimeOfDayUSec(){
    struct  timeval    tv;
    //struct  timezone   tz;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
    // printf("tv_sec:%ld\n",tv.tv_sec);
    //    printf("tv_usec:%ld\n",tv.tv_usec);
}

static std::string getTime(){
    std::time_t t;
    std::time ( &t );
    std::string tStr = std::ctime(&t);
    //去掉回车
    tStr.erase(tStr.size() - 1, 1);
    return tStr;
}

static void log(const std::string & msg){
    std::cout<<getTime()<<" : "<<msg<<std::endl;
}

static int string2int(const std::string& s){
    if(s.length() == 0){
        return 0;
    }
    std::istringstream is(s);
    int ret = 0;
    is>>ret;
    return ret;
}

static std::string int2string(int i){
    std::stringstream is;
    is<<i;
    return is.str();
}

static int getHostByName(const std::string& name, std::string& ip){
    struct hostent *hptr;
    char **pptr;
    char str[32];
    if( (hptr = gethostbyname(name.c_str()) ) == NULL )
    {
        return -1; /* 如果调用gethostbyname发生错误，返回1 */
    }
    pptr=hptr->h_addr_list;
    inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
    ip = str;
    return 0;
}


static void parseUrl(const std::string& url, std::string& host, std::string& path)
{

    size_t pos1 = 0;
    if(beginWith(url, "http://"))
    {
        pos1 = 7;
    }
    else if(beginWith(url, "https://"))
    {
        pos1 = 8;
    }
    size_t pos2 = url.find("/", pos1);
    if(pos2 == std::string::npos)
    {
        pos2 = url.length();
    }
    host = url.substr(pos1, pos2 - pos1);

    //path
    if(pos2 == url.length())
    {
        path = "/";
    }
    else
    {
        path = url.substr(pos2, url.length() - pos2);
    }

}
}

#endif // UTILS_H

