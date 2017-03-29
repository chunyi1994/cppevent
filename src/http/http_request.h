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

    void set_request_line(const std::string& method, const std::string& path, const std::string& version);
    void set_method(const std::string& method) { method_ = method; }
    void set_path(const std::string& path) { path_ = path; }
    void set_version(const std::string& version) { version_ = version; }
    void set_cookie(const std::string&key, const std::string& value);
    void set_data(const std::string&key, const std::string& value);
    void set_datas(const std::map<std::string, std::string>& datas) { datas_map_ = datas; }

    std::string method() const;
    std::string path() const;
    std::string version() const;
    std::string form(const std::string &key) const;
    std::string cookie(const std::string& key) const;


    std::string to_string() const;

    const std::string& body() const { return body_; }
    void append_body(const std::string& msg) { body_.append(msg); }
    void set_body(const std::string& body) { body_ = body; }

private:
    void parse_cookie();

private:
    std::string path_;
    std::string version_;
    std::string method_;
    std::string body_;
    std::map<std::string, std::string> datas_map_;
    std::map<std::string, std::string> cookies_map_;
};

HttpRequest default_request();
void default_request(HttpRequest& request);

}//namespace
}//namespace
#endif // HTTPREQUEST_H
