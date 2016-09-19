#include <functional>

#include "httpclient.h"
#include "utils.h"
namespace cppevent{

void parseUrl(const std::string& url, std::string& host, std::string& path);

HttpClient::HttpClient(EventLoop *loop) : tcpClient_(loop), message_()
{
    tcpClient_.setMessageCallback(std::bind(&HttpClient::handleMessage, this, std::placeholders::_1));
}

void HttpClient::addHeader(const std::string &name, const std::string &value)
{
    message_[name] = value;
}

void HttpClient::setMessageCallback(const HttpMessageCallback &cb)
{
    httpMessageCallback_ = cb;
    //还没想好callback怎么写
}

int HttpClient::get(const std::string &url, size_t port)
{
    std::string data;
    return get(url, data, port);
}

int HttpClient::post(const std::string &url, size_t port)
{
    std::string data;
    return post(url, data, port);
}

int HttpClient::get(const std::string &url, const std::string& data, size_t port)
{
    std::string host;
    std::string path;
    std::string ip;

    parseUrl(url, host, path);
    int code = getHostByName(host,ip);
    if(code < 0)
    {
        log("gethostbyname error.");
        return code;
    }
    if(!data.empty())
    {
        path = path + "?" + data;
    }
    message_.setRequestLine("GET", path, "HTTP/1.1");
    message_["Host"] = host;
    setDefaultHeaders(host);
    tcpClient_.setConnectionCallback([this](const ConnectionPtr& conn){
        std::string str = message_.toString();
        std::cout<<str<<std::endl;
        conn->send(str);
    });

    tcpClient_.connect(ip, port);

    return 0;
}

int HttpClient::post(const std::string &url, const std::string& data, size_t port)
{
    std::string host;
    std::string path;
    std::string ip;

    parseUrl(url, host, path);
    int code = getHostByName(host,ip);
    if(code < 0)
    {
        log("gethostbyname error.");
        return code;
    }
    message_.setRequestLine("POST", path, "HTTP/1.1");
    setDefaultHeaders(host);
    message_["Host"] = host;

    tcpClient_.connect(ip, port);
    tcpClient_.setConnectionCallback([this, data](const ConnectionPtr& conn){
        conn->send(message_.toString());
        conn->send(data);
    });
    return 0;
}

void HttpClient::setDefaultHeaders(const std::string& host)
{
    message_["Host"] = host;
    message_["Connection"] = "keep-alive";
    message_["Accept-Language"] = "zh-CN,zh;q=0.8";
    message_["Accept"] =  "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    message_["User-Agent"] = "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/51.0.2704.79 Chrome/51.0.2704.79 Safari/537.36";
    message_["Cache-Control"] = "max-age=0";
}

void HttpClient::handleMessage(const ConnectionPtr &conn)
{
    std::string msgStr = conn->readAll();
    std::cout<<msgStr<<std::endl;

    if(msgStr.length() < 11 || !beginWith(msgStr, "HTTP/1."))
    {
        return;
    }

    HttpMessage msg;
    msg.parse(msgStr, TYPE_HTTP_RESPONSE);
    //httpMessageCallback_(conn, msg);
}

void parseUrl(const std::string& url, std::string& host, std::string& path)
{

    size_t pos1 = 0;
    if(beginWith(url, "http://"))
    {
        pos1 = 7;
    }
    else if(beginWith(url, "https://"))
    {
        pos1 = 8;
    }
    size_t pos2 = url.find("/", pos1);
    if(pos2 == std::string::npos)
    {
        pos2 = url.length();
    }
    host = url.substr(pos1, pos2 - pos1);

    //path
    if(pos2 == url.length())
    {
        path = "/";
    }
    else
    {
        path = url.substr(pos2, url.length() - pos2);
    }

}


}
