#include <string>

#include "tiny_proxy.h"
#include "../utils.h"
#include "../tcpclient.h"
#include "../http_message.h"

namespace cppevent{

ProxyServer::ProxyServer(EventLoop* loop, size_t port) :
    loop_(loop),
    server_(loop, port),
    clientsMap_(),
    clientConnsMap_()
{
    server_.setMessageCallback(std::bind(&ProxyServer::handleMessage, this, std::placeholders::_1));
    server_.setConnectionCallback(std::bind(&ProxyServer::handleConnection, this, std::placeholders::_1));
}

void cppevent::ProxyServer::start()
{
    server_.start();
}

void setMessage(std::string& msg, std::string& host)
{
    HttpMessage httpMsg;
    httpMsg.parse(msg, TYPE_HTTP_REQUEST);
    std::string version = httpMsg.version();
    std::string url = httpMsg.path();
    std::string method = httpMsg.method();
    std::string path;
    parseUrl(url, host, path);
    httpMsg.setRequestLine(method, path, version);
    msg = httpMsg.toString();
}

void ProxyServer::handleMessage(const ConnectionPtr &conn)
{
    std::string msg = conn->readAll();
    if(beginWith(msg, "CONNECT"))
    {
        conn->send("HTTP/1.1 200 Connection Established\r\n\r\n");
        size_t pos = msg.find("\r\n\r\n");
        assert(pos != std::string::npos);
        msg = msg.substr(pos + 4, msg.length() - pos - 4);
    }

    if(beginWith(msg, "GET") || beginWith(msg, "POST"))
    {
        std::cout<<"---"<<msg<<"---"<<std::endl;
        std::string host;
        setMessage(msg, host);
        TcpClientPtr client = std::make_shared<TcpClient>(loop_);
        std::string ip;
        getHostByName(host, ip);

        client->setConnectionCallback(std::bind(
                     &ProxyServer::handleWebSiteConnection, this, std::placeholders::_1, conn, msg));

        client->setMessageCallback(
                     std::bind(&ProxyServer::handleWebSiteMessage, this, std::placeholders::_1));

        client->connect(ip, 80);

        clientsMap_[conn->fd()] = client;

        std::cout<<"#"<<msg<<"#"<<std::endl;
    }
    else
    {
        std::cout<<"*"<<msg<<"*"<<std::endl;
    }

}

void ProxyServer::handleConnection(const ConnectionPtr &conn)
{
    if(!conn->connecting())
    {
        auto iterFind = clientsMap_.find(conn->fd());
        if(iterFind != clientsMap_.end())
        {
            clientsMap_.erase(iterFind);
        }

    }

}

void ProxyServer::handleWebSiteConnection(
        const ConnectionPtr &webConn, const ConnectionPtr &clientConn, const std::string &msg)
{
    if(webConn->connecting())
    {
        std::cout<<"来啦!!!!"<<msg<<std::endl;
        clientConnsMap_[webConn->fd()] = clientConn;
        webConn->send(msg);
    }
    else
    {
        auto iterFind = clientConnsMap_.find(webConn->fd());
        if(iterFind != clientConnsMap_.end())
        {
            clientConnsMap_.erase(iterFind);
        }
    }
}

void ProxyServer::handleWebSiteMessage(const ConnectionPtr &webConn)
{
    std::string websiteMsg = webConn->readAll();
    std::cout<<"@"<<websiteMsg<<"@"<<std::endl;
//    std::string websiteMsg = webConn->readAll();
//    ConnectionPtr client = clientConnsMap_[webConn->fd()];
//    if(!client)
//    {
//        log("!client");

//        return;
//    }
//    std::cout<<"@"<<websiteMsg<<"@"<<std::endl;
//    client->send(websiteMsg);
}




}
