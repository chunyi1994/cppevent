#include "http_message.h"
#include "../logging/logger.h"
#include "../utils.h"
namespace net {
namespace http{

HttpMessage::HttpMessage() :
    headers_map_()
{
}

std::string &HttpMessage::operator[](const std::string &key) {
    return headers_map_[key];
}

void HttpMessage::add_header(const std::string &key, const std::string &value) {
    headers_map_[key] = value;
}

std::string parse_data(const std::map<std::string, std::string> &datas) {
    std::string result;
    for (auto& pair : datas) {
        result = result + pair.first + "=" + pair.second + "&";
    }
    if (!result.empty()) {
        result.erase(result.length() - 1, 1);
    }
    return result;
}

std::string HttpMessage::to_string() const {
    return "";
}

void HttpMessage::parse(const std::string &content) {
    if(content.length() == 0) {
        LOG_DEBUG<<"err";
        return;
    }
    std::string line;
    std::stringstream ss(content);
    std::getline(ss, line, '\r');
    size_t pos;
    size_t left;

    //让getline用\r来当换行符,则下一行的开头会是'\n'.
    //因为设置以后他不具备换行作用了.
    headers_map_.clear();
    while (std::getline(ss, line, '\r')) {
        //name
        pos = line.find(":");
        if (pos == std::string::npos){
            break;
        }
        std::string name = line.substr(1, pos - 1);
        //value
        left = pos + 2;
        pos = line.length() - 1;
        std::string value = line.substr(left, pos);
        //去掉左右空格
        utils::trim(value);
        headers_map_[name] = value;
    }
}

HttpMessage::HeadersMap::iterator HttpMessage::find(const std::__cxx11::string &key) {
    return headers_map_.find(key);
}

HttpMessage::HeadersMap::iterator HttpMessage::begin() {
    return headers_map_.begin();
}

HttpMessage::HeadersMap::iterator HttpMessage::end() {
    return headers_map_.end();
}

HttpMessage::HeadersMap::const_iterator HttpMessage::begin() const {
    return headers_map_.cbegin();
}

HttpMessage::HeadersMap::const_iterator HttpMessage::end() const
{
    return headers_map_.cend();
}

HttpMessage::HeadersMap::const_iterator HttpMessage::cbegin() const
{
    return headers_map_.cbegin();
}

HttpMessage::HeadersMap::const_iterator HttpMessage::cend() const {
    return headers_map_.cend();
}




}//namespace http
}//namespace net

