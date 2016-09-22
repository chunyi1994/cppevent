#include <functional>

#include "httpclient.h"
#include "utils.h"
namespace cppevent{

void parseUrl(const std::string& url, std::string& host, std::string& path);

HttpClient::HttpClient(EventLoop *loop) : tcpClient_(loop), request_(), response_()
{
    tcpClient_.setMessageCallback(std::bind(&HttpClient::handleMessage, this, std::placeholders::_1));
}

void HttpClient::addHeader(const std::string &name, const std::string &value)
{
    request_[name] = value;
}

void HttpClient::setMessageCallback(const HttpMessageCallback &cb)
{
    httpMessageCallback_ = cb;
}

void HttpClient::setHttpMessage(const HttpMessage &msg)
{
    request_ = msg;
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
    request_.setRequestLine("GET", path, "HTTP/1.1");
    request_["Host"] = host;
    setDefaultHeaders(host);
    tcpClient_.setConnectionCallback([this](const ConnectionPtr& conn){
        if(conn->connecting())
        {
            std::string str = request_.toString();
            //std::cout<<str<<std::endl;
            conn->send(str);
        }
    });

    code = tcpClient_.connect(ip, port);

    return code;
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
    request_.setRequestLine("POST", path, "HTTP/1.1");
    setDefaultHeaders(host);
    request_["Host"] = host;


    tcpClient_.setConnectionCallback([this, data](const ConnectionPtr& conn){
        if(conn->connecting()){
            conn->send(request_.toString());
            conn->send(data);
        }

    });

    code =  tcpClient_.connect(ip, port);
    return code;
}

void HttpClient::setDefaultHeaders(const std::string& host)
{
    request_["Host"] = host;
    request_["Connection"] = "keep-alive";
    request_["Accept-Language"] = "zh-CN,zh;q=0.8";
    request_["Accept"] =  "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    request_["User-Agent"] = "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/51.0.2704.79 Chrome/51.0.2704.79 Safari/537.36";
    request_["Cache-Control"] = "max-age=0";
}

void HttpClient::handleMessage(const ConnectionPtr &conn)
{
    std::string msgStr = conn->readAll();
    if(msgStr.length() > 11 && beginWith(msgStr, "HTTP/1."))  //11随便打的
    {
        size_t pos = msgStr.find("\r\n\r\n");
        if(pos != std::string::npos)
        {
            pos += 4;
            std::string httpResponseStr = msgStr.substr(0, pos);
            msgStr = msgStr.substr(pos, msgStr.length());
            response_.parse(httpResponseStr, TYPE_HTTP_RESPONSE);
        }
    }

    if(httpMessageCallback_)
    {
        httpMessageCallback_(response_, msgStr);
    }

    auto iterFind = response_.find("Connection");
    if(iterFind != response_.end() && iterFind->second == "close")
    {
        tcpClient_.shutdown();
    }

}


}
