#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H
#include <map>
#include <string>
namespace net {
namespace http{

const int TYPE_HTTP_RESPONSE = 0;
const int TYPE_HTTP_REQUEST = 1;


class HttpMessage
{
public:
    typedef std::map<std::string, std::string> HeadersMap;
public:
    HttpMessage();
    std::string& operator[](const std::string& key);
    std::string& header(const std::string& key) { return operator[](key); }
    void add_header(const std::string& key,const  std::string& value);
    //这个操作比较费时,每次调用都会重新解析一次
    std::string to_string() const;
    //
    void parse(const std::string &content);

    HeadersMap::iterator find(const std::string& key);
    HeadersMap::iterator begin();
    HeadersMap::iterator end();
    HeadersMap::const_iterator begin() const;
    HeadersMap::const_iterator end() const;
    HeadersMap::const_iterator cbegin() const;
    HeadersMap::const_iterator cend() const;

protected:
    HeadersMap headers_map_;
};
std::string parse_data(const std::map<std::string, std::string> &datas);



}//namespace http
}//namespace net

#endif // HTTPMESSAGE_H
