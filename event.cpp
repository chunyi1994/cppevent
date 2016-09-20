#include "event.h"
#include "utils.h"
namespace cppevent{
Event::Event(int fd, uint32_t event):fd_(fd), events_(event){

}


uint32_t Event::events() const
{
    return events_;
}

void Event::setReadCallback(const EventCallback &cb)
{
    readCallback_ = cb;
}

void Event::setWriteCallback(const EventCallback &cb)
{
    writeCallback_ = cb;
}

void Event::setErrorCallback(const EventCallback &cb)
{
    errorCallback_ = cb;
}

void Event::setRevents(uint32_t ev){
    revents_ = ev;
}

void Event::setCloseCallback(const EventCallback &cb)
{
    closeCallback_ = cb;
}

int Event::fd() const
{
    return fd_;
}

void Event::setFd(int fd)
{
    fd_ = fd;
}


void Event::handleEvent(){
//    if(revents_ & EPOLLHUP && closeCallback_){
//        closeCallback_();
//    }

    if(revents_ & EPOLLRDHUP && closeCallback_)
    {
        //当对方close套接字的时候,会调用这里
        closeCallback_();
    }

    if(revents_ & EPOLLIN && readCallback_)
    {
        readCallback_();
    }

    if(revents_ & EPOLLOUT && writeCallback_)
    {
        writeCallback_();
    }

    if(revents_ & EPOLLERR && errorCallback_)
    {
        errorCallback_();
    }
}
//http://blog.clawz.me/2009/03/12/09-epoll-event-test/
//1、listen fd，有新连接请求，触发EPOLLIN。
//2、对端发送普通数据，触发EPOLLIN。
//3、带外数据，只触发EPOLLPRI。
//4、对端正常关闭（程序里close()，shell下kill或ctr+c），触发EPOLLIN和EPOLLRDHUP，但是不触发EPOLLERR和EPOLLHUP。
//    关于这点，以前一直以为会触发EPOLLERR或者EPOLLHUP。
//    再man epoll_ctl看下后两个事件的说明，这两个应该是本端（server端）出错才触发的。
//5、对端异常断开连接（只测了拔网线），没触发任何事件。

//EPOLLIN
//              The associated file is available for read(2) operations.
//EPOLLOUT
//              The associated file is available for write(2) operations.
//EPOLLRDHUP
//              Stream  socket peer closed connection, or shut down writing half
//              of connection.  (This flag is especially useful for writing sim-
//              ple code to detect peer shutdown when using Edge Triggered moni-
//              toring.)
//EPOLLPRI
//              There is urgent data available for read(2) operations.
//EPOLLERR
//              Error condition happened  on  the  associated  file  descriptor.
//              epoll_wait(2)  will always wait for this event; it is not neces-
//              sary to set it in events.
//EPOLLHUP
//              Hang  up   happened   on   the   associated   file   descriptor.
//              epoll_wait(2)  will always wait for this event; it is not neces-
//              sary to set it in events.
//EPOLLET
//              Sets  the  Edge  Triggered  behavior  for  the  associated  file
//              descriptor.   The default behavior for epoll is Level Triggered.
//              See epoll(7) for more detailed information about Edge and  Level
//              Triggered event distribution architectures.
//EPOLLONESHOT (since Linux 2.6.2)
//              Sets  the  one-shot behavior for the associated file descriptor.
//              This means that after an event is pulled out with  epoll_wait(2)
//              the  associated  file  descriptor  is internally disabled and no
//              other events will be reported by the epoll interface.  The  user
//              must  call  epoll_ctl() with EPOLL_CTL_MOD to re-enable the file
//              descriptor with a new event mask.

}
