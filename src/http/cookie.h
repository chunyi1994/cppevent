#ifndef COOKIE
#define COOKIE
#include <string>
#include <utility>
namespace net {
namespace http {

class Cookie
{
public:
    Cookie(std::string key,std::string value, std::string path = "",std::string domain = "", std::string expires = ""):
        key_(std::move(key)),
        value_(std::move(value)),
        path_(std::move(path)),
        domain_(std::move(domain)),
        expires_(std::move(expires))
    {}

    std::string to_string() const {
        std::string ret = key_ + "=\"" + value_ + "\"";
        if (!path_.empty()) {
            ret += "; path =\"" + path_ + "\"";
        }

        if (!domain_.empty()) {
            ret += "; domain =\"" + domain_ + "\"";
        }

        if(!expires_.empty()) {
            ret += "; expires =\"" + expires_ + "\"";
        }

        return ret;
    }

private:
    std::string key_;
    std::string value_;
    std::string path_;
    std::string domain_;
    std::string expires_;
};
} //namespace
} //namespace

#endif // COOKIE

