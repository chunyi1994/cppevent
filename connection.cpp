#include "connection.h"
#include "eventloop.h"
#include "utils.h"
#include <stdio.h>
#include <errno.h>
namespace cppevent{
const int DEFAULT_MAX_BUFFER_SIZE = 2048;

Connection::Connection(EventLoop *loop, int fd) :
    connfd_(fd),
    event_(fd,  EPOLLIN | EPOLLHUP | EPOLLRDHUP),
    connecting_(false),
    peer_(),
    bufferMaxSize_(DEFAULT_MAX_BUFFER_SIZE)
{
    loop->addEvent(&event_);
    event_.setReadCallback(std::bind(&Connection::handleRead, this));
    event_.setCloseCallback(std::bind(&Connection::handleClose, this));
    event_.setErrorCallback(std::bind(&Connection::handleError, this));
    event_.setWriteCallback(std::bind(&Connection::handleWrite, this));
}

Connection::~Connection()
{
    log("~Connection and close fd.");
}

void Connection::setMessageCallback(const MessageCallback &cb)
{
    messageCallback_ = cb;
}

void Connection::setConnectionCallback(const ConnectionCallback &cb)
{
    connectionCallback_ = cb;
}

void Connection::setWriteCallback(const MessageCallback &cb)
{
    writeCallback_ = cb;
}

int Connection::fd() const
{
    return connfd_.fd();
}

bool Connection::connecting() const
{
    return connecting_;
}

std::string Connection::read(size_t len)
{
    return buffer_.read(len);
}

std::string Connection::readAll()
{
    return buffer_.read(buffer_.size());
}

bool Connection::readLine(std::string &line)
{
    return buffer_.readLine(line);
}

bool Connection::readLine(std::string &str, char br)
{
    return buffer_.readLine(str, br);
}

size_t Connection::readSize() const
{
    return buffer_.size();
}

void Connection::send(const std::string &msg)
{
    int wirten = 0;
    int n = 0;
    while(wirten < msg.length()){
        n = ::write(connfd_.fd(),  msg.c_str() + wirten, msg.length() - wirten);
        if(n < 0){
            if (n == -1 && errno != EAGAIN) {
                perror("write error");
            }
            break;
        }
        wirten += n;
    }
}

void Connection::setConnectionStatus(bool isConnecting)
{
    connecting_ = isConnecting;
}

void Connection::shutdown()
{
    handleClose();
}

void Connection::setAddress(const TcpAddress &addr)
{
    peer_ = addr;
}

const TcpAddress &Connection::address() const
{
    return peer_;
}

void Connection::handleClose()
{
    connecting_ = false;
    if(connectionCallback_){
        connectionCallback_(shared_from_this());
    }
}

void Connection::handleRead()
{
    int n = 0;
    bool closeFlag = false;
    char buf[1024];
    while(1){
        //每当设定的buffer到了maxsize的时候,便停止读取缓冲区的数据
        //这样做是因为,如果一直被发送数据,
        //那么一直在while里面,则buffer_可能会无限增大
        //如果对方持续发送1G才停,内存可能会爆掉
        if(buffer_.size() >= bufferMaxSize_)
        {
            break;
        }

        n = ::read(connfd_.fd(), buf, 1023);
        buf[n] = '\0';
        if(n < 0){
            n = 0;
            if(errno == ECONNRESET){
                log("ECONNRESET");
                closeFlag = true;
            }
            else if(errno == EAGAIN){
                log("EAGAIN");
            }
            break;

        }else if(n == 0 && errno != EINTR){
            //如果errno == EINTR 则说明recv函数是由于程序接收到信号后返回的，
            //socket连接还是正常的，不应close掉socket连接。
            //一般来说,read到0算是对方关闭了套接字的写端,
            //但是正确使用epoll的话,有更优雅的回调方式
            //所以这里即使读到了0,也不close,而是等epoll发出EPOLLRDHUP再调用closecallback
            closeFlag = true;
            break;
        }

        buffer_.append(buf, n);

    }


    if(messageCallback_)
    {
        messageCallback_(shared_from_this());
    }
}

void Connection::handleError()
{
    int       err = 0;
    socklen_t errlen = sizeof(err);
    if (::getsockopt(connfd_.fd(), SOL_SOCKET, SO_ERROR, (void *)&err, &errlen) == 0)
    {
        printf("error = %s\n", ::strerror(err));
    }
    log("handleError");
    handleClose();
}

void Connection::handleWrite()
{
    if(writeCallback_)
    {
        writeCallback_(shared_from_this());
    }
}

}
