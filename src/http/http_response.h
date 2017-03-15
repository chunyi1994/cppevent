#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include "http_message.h"
#include <vector>
#include "cookie.h"
namespace net {
namespace http {
class HttpResponse : public HttpMessage
{
public:
    HttpResponse();
    void parse(const std::string &content);
    void set_status_line(const std::string& version, int status_code, const std::string& status);
    void set_cookie(std::string key, std::string value, std::string path = "",std::string domain = "", std::string expires = "");

    std::string status_str() const;
    std::string version() const;
    int status_code() const;
    std::string to_string() const;

private:
    std::vector<Cookie> cookies_;
    std::string version_;
    std::string status_;
    int status_code_;
};
}//namespace
}//namespace

#endif // HTTPRESPONSE_H
