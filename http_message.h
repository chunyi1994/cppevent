#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H
#include <string>
#include <map>
namespace cppevent{

const int TYPE_HTTP_RESPONSE = 0;
const int TYPE_HTTP_REQUEST = 1;

class HttpMessage
{
    typedef std::map<std::string, std::string> HeadersMap;

public:
    HttpMessage();
    std::string& operator[](const std::string& key);

    void addHeaders(const std::string& key,const  std::string& value);
    void setStatusLine(const std::string& version, int statusCode,const std::string& status);
    void setRequestLine(const std::string& method, const std::string& path, const std::string& version);
    std::string toString() const;    //这个操作比较费时,每次调用都会重新解析一次
    std::string method() const;
    std::string path() const;
    std::string version() const;
    int statusCode() const;
    void parse(const std::string &content, int type);

    HeadersMap::iterator find(const std::string& key);

    HeadersMap::iterator begin();
    HeadersMap::iterator end();

    HeadersMap::const_iterator begin() const;
    HeadersMap::const_iterator end() const;

    HeadersMap::const_iterator cbegin() const;
    HeadersMap::const_iterator cend() const;

private:
    HeadersMap headersMap_;
    int type_;
    std::string path_;
    std::string version_;
    std::string method_;
    std::string status_;
    int statusCode_;
};

}

/*
        用来生成一个http msg报文
        HttpMessage msg;
        msg.setRequestLine("GET", "/", "HTTP/1.1");
        msg["Host"] = "cn.bing.com";          //msg.addHeaders("Host","cn.bing.com");
        msg["Connection"] = "keep-alive";  //msg.addHeaders("Connection","keep-alive");
        std::cout << msg.toString() << std::endl;
*/

/*
        用来解析一个http msg报文
        HttpMessage msg;
        string msgStr = "GET / HTTP/1.1\r\nHost: cn.bing.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\n\r\n";
        msg.parse(msgStr, TYPE_HTTP_REQUEST);
        std::cout<< msg["Host"] << std::endl;

        for(auto w : msg){
            cout<<w.first<<endl;
            cout<<w.second<<endl;
        }

        for(auto iter = msg.begin();
                iter !+ msg.end();
                iter++){
            cout<<iter->first<<endl;
            cout<<iter->second<<endl;
        }
*/



#endif // HTTPMESSAGE_H
