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
    body_(),
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

//usage: set_cookie().key("key1").value("value222");
CookieBuilder HttpResponse::set_cookie() {
    return CookieBuilder(*this);
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

HttpResponse default_response() {
        HttpResponse response;
        response.set_version("HTTP/1.1");
        response["Server"] = "Cppevent Server 1.0";
        response["Content-Type"] = "text/html";
        return response;
}

HttpResponse ok_response() {
    HttpResponse response =  default_response();
    response.set_status("OK");
    response.set_status_code(200);
    return response;
}

HttpResponse bad_request() {
    HttpResponse response = default_response();
    response.set_status_line("HTTP/1.1", 400, "Bad Request");
    return response;
}

HttpResponse not_found() {
    HttpResponse response = default_response();
    response.set_status_line("HTTP/1.1", 404, "Not found");
    return response;
}

std::string content_type(const std::string &path) {
    size_t dot_pos = path.find(".");
    if(dot_pos == std::string::npos){
        return "text/html";
    }
    std::string ext = path.substr(dot_pos, 4);
    if (ext == ".png") {
        return "image/png";
    } else if (ext == ".css") {
        return "text/css";
    } else if (ext == ".jpg") {
        return "image/jpeg";
    } else if (ext == ".gif") {
        return "image/gif";
    }  else if (ext == ".htm") {
        return "text/html";
    }  else {
        return "text/html";
    }
}
}//namespace
}//namespace
