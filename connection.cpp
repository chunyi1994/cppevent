#include "connection.h"
#include "eventloop.h"
#include "utils.h"
#include <stdio.h>
#include <errno.h>
namespace cppevent{
const int DEFAULT_MAX_BUFFER_SIZE = 2048;

Connection::Connection(EventLoop *loop, int fd) :
    connfd_(fd),
    event_(fd,  EPOLLIN | EPOLLET | EPOLLOUT | EPOLLHUP),
    connecting_(false),
    bufferMaxSize_(DEFAULT_MAX_BUFFER_SIZE),
    bufferSize_(0)
{
    loop->addEvent(&event_);
    event_.setReadCallback(std::bind(&Connection::handleRead, this));
    event_.setCloseCallback(std::bind(&Connection::handleClose, this));
    event_.setErrorCallback(std::bind(&Connection::handleError, this));
    event_.setWriteCallback(std::bind(&Connection::handleWrite, this));
}

Connection::~Connection(){
    //log("~Connection");
}

void Connection::setMessageCallback(const MessageCallback &cb){
    messageCallback_ = cb;
}

void Connection::setConnectionCallback(const ConnectionCallback &cb){
    connectionCallback_ = cb;
}

void Connection::setWriteCallback(const MessageCallback &cb)
{
    writeCallback_ = cb;
}

int Connection::fd() const{
    return connfd_.fd();
}

bool Connection::connecting() const{
    return connecting_;
}

std::string Connection::read(size_t len){
    return buffer_.read(len);
}

std::string Connection::readAll(){
    return buffer_.read(buffer_.size());
}

bool Connection::readLine(std::string &line){
    return buffer_.readLine(line);
}

bool Connection::readLine(std::string &str, char br){
    return buffer_.readLine(str, br);
}

size_t Connection::readSize() const{
    return buffer_.size();
}

void Connection::send(const std::string &msg){
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

void Connection::setConnectionStatus(bool isConnecting){
    connecting_ = isConnecting;
}

void Connection::handleClose(){
    connecting_ = false;
    if(connectionCallback_){
        connectionCallback_(shared_from_this());
    }
}

void Connection::handleRead(){
    bool closeFlag = false;
    int n = 0;
    char buf[1024];
    while(1){
        n = ::read(connfd_.fd(), buf, 1023);
        //std::cout<<"n"<<n<<std::endl;
        buf[n] = '\0';
        if(n < 0){
            n = 0;
            //perror("read error");
            if(errno == ECONNRESET){
                closeFlag = true;
            }
            if(errno == EAGAIN){
                log("eagain");
            }
            break;

        }else if(n == 0 && errno != EINTR){
            std::cout<<errno<<std::endl;
            //如果errno == EINTR 则说明recv函数是由于程序接收到信号后返回的，
            //socket连接还是正常的，不应close掉socket连接。
            closeFlag = true;
            break;
        }

        buffer_.append(buf, n);
    }


    if(messageCallback_){
        messageCallback_(shared_from_this());
    }

    if(closeFlag){
       handleClose();
    }

}

void Connection::handleError()
{
    log("handleError");
}

void Connection::handleWrite()
{
    if(writeCallback_){
        writeCallback_(shared_from_this());
    }
}

}
