#ifndef PROXY_TEST_H
#define PROXY_TEST_H
#include "../tcpserver.h"
#include "../eventloop.h"
#include "../connection.h"
#include "utils.h"
#include <functional>
#include <iostream>
#include <map>
#include <assert.h>
#include <memory>
//还没写完,不知道哪里出了问题
//写的是一个翻墙的小程序,测试失败

namespace cppevent{

class TcpClient;

class ProxyServer
{
    typedef std::shared_ptr<TcpClient> TcpClientPtr;
public:
    ProxyServer(EventLoop* loop, size_t port);
    void start();

private:
    void handleMessage(const ConnectionPtr& conn);
    void handleConnection(const ConnectionPtr& conn);
    void handleWebSiteConnection(const ConnectionPtr& webConn, const ConnectionPtr& clientConn, const std::string& msg);
    void handleWebSiteMessage(const ConnectionPtr& webConn);

    EventLoop* loop_;
    TcpServer server_;
    std::map<int, TcpClientPtr> clientsMap_;
    std::map<int, ConnectionPtr> clientConnsMap_;
};
}
#endif // PROXY_TEST_H

