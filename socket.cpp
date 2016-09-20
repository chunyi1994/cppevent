#include "socket.h"

namespace cppevent{

Socket::Socket(int fd) : fd_(fd){}

Socket::~Socket()
{
    if(fd_ != -1)
    {
        ::close(fd_);
    }
}

void Socket::setFd(int fd)
{
    if(fd_ != -1)
    {
        ::close(fd_);
    }
    fd_ =fd;
}

int Socket::fd() const{
    return fd_;
}

}
