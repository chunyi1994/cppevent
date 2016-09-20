#include <functional>
#include <utils.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>

#include "tcpclient.h"
#include "eventloop.h"
#include "connection.h"

using std::cout;
using std::endl;

namespace cppevent{

TcpClient::TcpClient(EventLoop *loop) : loop_(loop)
{

}

int tcpInit(const string &ip, int port)
{
    int sockfd, status, save_errno;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = inet_aton(ip.c_str(), &server_addr.sin_addr);
    if( status == 0 ) //the server_ip is not valid value
    {
        errno = EINVAL;
        perror("inet error");
        return -1;
    }
    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 ){
        perror("socket error");
        return sockfd;
    }

    status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if( status == -1 )
    {
        perror("connect error");
        save_errno = errno;
        ::close(sockfd);
        errno = save_errno; //the close may be error
        return -1;
    }
    return sockfd;
}

void TcpClient::connect(const std::string &ip, int port)
{
   int sockfd =  tcpInit(ip,port);
   if(sockfd < 0){
       log("connect err:");
       return;
   }
   setnonblocking(sockfd);
   TcpAddress tcpAddr(ip, port);
   connPtr_->setAddress(tcpAddr);
   connPtr_ = std::make_shared<Connection>(loop_, sockfd);
   connPtr_->setMessageCallback(messageCallback_);

    if(connectionCallback_)
    {
        connectionCallback_(connPtr_);
    }
    else
    {
        log("no connectionCallback");
    }
}

void TcpClient::setConnectionCallback(const ConnectionCallback &cb)
{
    connectionCallback_ = cb;

}

void TcpClient::setMessageCallback(const MessageCallback &cb)
{
    messageCallback_ = cb;
}

}
