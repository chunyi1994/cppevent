#ifndef PROXY_TEST_H
#define PROXY_TEST_H
#include "tcpserver.h"
#include "eventloop.h"
#include "connection.h"
#include "utils.h"
#include <functional>
#include <iostream>
#include <map>
#include <assert.h>
#include <algorithm>
#include <fstream>
using namespace std;
using namespace cppevent;
string content;

//还没写完,
//写的是一个翻墙的小程序,测试失败
class ProxyServer{
public:
    ProxyServer(EventLoop* loop, size_t port) : loop_(loop), server_(loop, port)
    {
        server_.setConnectionCallback(
                    std::bind(&ProxyServer::connCallback, this, std::placeholders::_1));
        server_.setMessageCallback(
                    std::bind(&ProxyServer::msgCallback, this, std::placeholders::_1));
    }
    void start(){
        string html;
        html = html +
                "<html>"+
                "<head><title>200 good Request</title></head>"+
                "<body bgcolor=\"white\">"+
                "<center><h1>666 Bad Request</h1></center>"+
                "<hr><center>squid/3..3</center>"+
                "</body>"+
                "</html>";

        content = content + "HTTP/1.1 200 ok\r\n"+
                "Server: squid/3.4.3\r\n"+
                "Content-Type: text/html\r\n"+
                "Content-Length: " + int2string(html.length()) +"\r\n"+
                "Connection: close\r\n\r\n" + html;

        server_.start();
    }

private:
    void msgCallback(const ConnectionPtr& conn){
        std::string msg = conn->readAll();
        std::cout<<"msg:"<<msg<<endl;

        if(msg.substr(0, 7) == "CONNECT"){
            conn->send("HTTP/1.1 200 Connection Established\r\n\r\n");
        }else{
            conn->send(content);
        }

    }

    void connCallback(const ConnectionPtr& conn){
        if(conn->connecting()){
            log("New Connection");

        }else{
            log("Connection Close");

        }
    }

    EventLoop* loop_;
    TcpServer server_;
};
#endif // PROXY_TEST_H

