#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <string>

#include "tcpclient.h"
#include "http_message.h"
namespace cppevent{

class EventLoop;

class HttpClient
{
public:
    HttpClient(EventLoop *loop);
    void addHeader(const std::string& name, const std::string& value);
    void setMessageCallback(const MessageCallback& cb);
    int get(const std::string&url, size_t port = 80);
    int get(const std::string&url, const std::string& data, size_t port = 80);
    int post(const std::string&url, size_t port = 80);
    int post(const std::string&url, const std::string& data, size_t port = 80);

private:
    void setDefaultHeaders(const std::string& host);
    void handleMessage(const ConnectionPtr& conn);

    TcpClient tcpClient_;
    HttpMessage message_;
    //MessageCallback cb_;

};

}
#endif // HTTPCLIENT_H
