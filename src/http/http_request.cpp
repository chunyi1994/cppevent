#include "http_request.h"
#include "../utils.h"
#include <assert.h>
namespace net {
namespace http {

HttpRequest::HttpRequest() :
    path_(),
    version_(),
    method_(""),
    body_(),
    datas_map_(),
    cookies_map_()
{}

void HttpRequest::parse(const std::string &content) {
    if(content.length() == 0) {
        LOG_DEBUG<<"err";
        return;
    }
    std::string line;
    std::stringstream ss(content);
    std::getline(ss, line, '\r');
    size_t pos;
    size_t left;
    //method
    pos = line.find(" ");
    if(pos == std::string::npos) {
        LOG_DEBUG<<"err";
        return;
    }
    std::string method = line.substr(0, pos);
    if (method == "get" || method == "GET") {
        method_ = "GET";
    } else if (method == "post" || method == "POST")  {
        method_ = "POST";
    } else if (method == "put" || method == "PUT") {
        method_ = "PUT";
    } else {
        //todo
        method_ = "OTHER";
    }
    //path
    left = pos + 1;
    pos = line.find(" ", left);
    if (pos == std::string::npos) {
        LOG_DEBUG<<"err";
        return;
    }
    path_ = line.substr(left, pos - left);
    //version
    left = pos + 1;
    pos = line.length();
    version_ = line.substr(left, pos - left);

    HttpMessage::parse(content);
    parse_cookie();
    pos = content.find("\r\n\r\n");
    if (pos != std::string::npos && pos + 4 < content.length()) {
        std::string data = content.substr(pos + 4, content.length() - pos - 4);
        body_ = std::move(data);
    }
}

void HttpRequest::parse_data() {
    std::string datas;
    if (method_ == "POST" && header("Content-Type") == "application/x-www-form-urlencoded") {
        datas = body_;
    } else if (method_ == "GET") {
        std::size_t pos = path_.find("?");
        if (pos == std::string::npos || pos == path_.length() - 1) {
            return;
        }
        datas = path_.substr(pos + 1, path_.length() - (pos + 1));
    } else {
        return;
    }

    std::vector<std::string> vec = utils::split(datas, '&');
    for (const auto& str : vec) {
        size_t pos = str.find("=");
        if(pos == std::string::npos) {
            continue;
        }
        std::string key = str.substr(0, pos);
        std::string value = str.substr(pos + 1, str.length() - pos - 1);
        datas_map_[key] = value;
    }
}

void HttpRequest::set_request_line(const std::string &method,
                                   const std::string &path,
                                   const std::string &version) {
    method_ = method;
    path_ = path;
    version_ = version;
}

std::string HttpRequest::method() const {
    return method_;
}

std::string HttpRequest::path() const {
    return path_;
}

std::string HttpRequest::version() const {
    return version_;
}

std::string HttpRequest::form(const std::string &key) const {
    auto iter = datas_map_.find(key);
    if (iter != datas_map_.end()) {
        return iter->second;
    } else {
        return "";
    }
}

std::string HttpRequest::cookie(const std::string &key) const {
    auto iter = cookies_map_.find(key);
    if (iter == cookies_map_.end()) {
        return "";
    }
    return iter->second;
}

void HttpRequest::set_cookie(const std::string &key, const std::string &value) {
    cookies_map_[key] =  value;
}

void HttpRequest::set_data(const std::string &key, const std::string &value)
{
    datas_map_[key] =  value;
}

std::string HttpRequest::to_string() const {
    std::string path = path_;
    //    if (method_ == "GET" && !body_.empty()) {
    //        path = path_ + "?" + body_;
    //    }
    std::string line = method_ + " " + path + " " + version_ + "\r\n";
    std::string msg(line);

    for (const auto &pair : headers_map_) {
        line = pair.first + ": " + pair.second + "\r\n";
        msg.append(line);
    }
    //todo 需要加强
    if (!cookies_map_.empty()) {
        msg.append("Cookie: ");
        for (const auto& pair : cookies_map_) {
            line = pair.first + "=" + pair.second + ";";
            msg.append(line);
        }
        msg.erase(msg.size() - 1, 1);
        msg.append("\r\n");
    }
    msg.append("\r\n");
    return msg;
}

void HttpRequest::parse_cookie() {
    //parse cookies
    std::string cookie = headers_map_["Cookie"];
    if (cookie.empty())  {
        return;
    }
    std::vector<std::string> cookies_vec = utils::split(cookie, ';');
    for(const auto& cookie_str : cookies_vec) {
        size_t pos = cookie_str.find("=");
        if(pos == std::string::npos) {
            continue;
        }
        std::string key = cookie_str.substr(0, pos);
        std::string value = cookie_str.substr(pos + 1, cookie_str.length() - pos - 1);
        utils::trim(key);
        utils::trim(value);
        utils::trim_quo(value);
        cookies_map_[key] = value;
    }
}

HttpRequest default_request() {
    HttpRequest request;
    default_request(request);
    return request;
}

void default_request(HttpRequest &request) {
    request["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    request["Accept-Language"] = "zh-CN,zh;q=0.8";
    request["Cache-Control"] = "0";
    request["Connection"] = "close";
    request["Content-Length"] = "0";
    request.set_version("HTTP/1.1");
    request["User-Agent"] = "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/51.0.2704.79 Chrome/51.0.2704.79 Safari/537.36";
}

}//namespace

}//namespace

