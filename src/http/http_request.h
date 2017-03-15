#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include "http_message.h"
namespace net {
namespace http {


class HttpRequest : public HttpMessage {
public:
    HttpRequest();

    void parse(const std::string &content);
    void parse_data();  //框架不会主动 这个，需要用户自行调用

    void set_raw_data(std::string data);
    void set_request_line(const std::string& method, const std::string& path, const std::string& version);
    void set_cookie(const std::string&key, const std::string& value);
    void set_data(const std::string&key, const std::string& value);

    std::string method() const;
    std::string path() const;
    std::string version() const;
    std::string form(const std::string &key) const;
    std::string cookie(const std::string& key) const;


    std::string to_string() const;
    const std::string& raw_data() const;
    void append_raw_data(const std::string& msg);

private:
    void parse_cookie();

private:
    std::string path_;
    std::string version_;
    std::string method_;
    std::string raw_data_;
    std::map<std::string, std::string> datas_map_;
    std::map<std::string, std::string> cookies_map_;
};



}//namespace
}//namespace
#endif // HTTPREQUEST_H
