#ifndef CHATROOM_H
#define CHATROOM_H
#include "tcpserver.h"
#include "eventloop.h"
#include "connection.h"
#include <functional>
#include <iostream>
#include <map>
#include <assert.h>
#include <algorithm>
using namespace std;
using namespace cppevent;
//一个测试这个网络库是否能正常工作的小server
//功能是:
//几个telnet连接上这个server,然后一个窗口发送消息,其余窗口都能收到
//类似于一个聊天室的功能
class ChatRoomServer{
public:
    ChatRoomServer(EventLoop* loop, size_t port) : loop_(loop), server_(loop, port)
    {
        server_.setConnectionCallback(
                    std::bind(&ChatRoomServer::connCallback, this, std::placeholders::_1));
        server_.setMessageCallback(
                    std::bind(&ChatRoomServer::msgCallback, this, std::placeholders::_1));
    }
    void start(){
        server_.start();
    }

private:
    void msgCallback(const ConnectionPtr& conn){
        std::string msg = conn->readAll();
        for_each(conns_.begin(), conns_.end(), [&](std::pair<int, ConnectionPtr> p){
                p.second->send(msg);
        });
    }

    void connCallback(const ConnectionPtr& conn){
        if(conn->connecting()){
            log("New Connection");
            conns_[conn->fd()] = conn;
        }else{
            log("Connection Close");
            auto iter = conns_.find(conn->fd());
            assert(iter != conns_.end());
            conns_.erase(iter);
        }

    }

    EventLoop* loop_;
    TcpServer server_;
    std::map<int,ConnectionPtr> conns_;
};

#endif // CHATROOM_H

