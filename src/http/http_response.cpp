#include "http_response.h"
#include "../utils.h"
#include "../logging/logger.h"
#include <assert.h>
namespace net {
namespace http {
HttpResponse::HttpResponse() :
    cookies_(),
    version_(),
    status_(),
    status_code_(0)
{

}

void HttpResponse::parse(const std::string &content) {
    if (content.length() == 0) {
        LOG_DEBUG<<"err";
        return;
    }
    std::string line;
    std::stringstream ss(content);
    std::getline(ss, line, '\r');
    size_t pos;
    size_t left;

    //version
    pos = line.find(" ");
    if(pos == std::string::npos)  {
       LOG_DEBUG<<"err";
        return;
    }
    version_ = line.substr(0, pos);
    //status code
    left = pos + 1;
    pos = line.find(" ", left);
    if (pos == std::string::npos) {
        LOG_DEBUG<<"err";
        return;
    }
    status_code_ = utils::to_int(line.substr(left, pos));
    HttpMessage::parse(content);
}

void HttpResponse::set_status_line(const std::string &version,
                                   int status_code,
                                   const std::string &status) {
    version_ = version;
    status_code_ = status_code;
    status_ = status;
}

std::string HttpResponse::to_string() const {
    std::string line = version_ + " " + utils::to_string(status_code_) + " " + status_ + "\r\n";

    std::string ret(line);

    for(const auto &w : headers_map_){
        line = w.first + ": " + w.second + "\r\n";
        ret.append(line);
    }

    for(const auto& w : cookies_) {
        line = "Set-Cookie: " + w.to_string() + "\r\n";
        ret.append(line);
    }
    ret.append("\r\n");
    return ret;
}

void HttpResponse::set_cookie(std::string key,
                              std::string value,
                              std::string path,
                              std::string domain,
                              std::string expires) {
    cookies_.push_back(Cookie(key,value, path ,domain, expires));
}

std::string HttpResponse::status_str() const {
    return status_;
}

std::string HttpResponse::version() const
{
    return version_;
}

int HttpResponse::status_code() const
{
    return status_code_;
}
}//namespace
}//namespace
