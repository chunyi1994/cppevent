#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include "http_message.h"
#include <vector>
#include <memory>
#include "cookie.h"
namespace net {
namespace http {

class CookieBuilder;


class HttpResponse : public HttpMessage
{
    friend class CookieBuilder;
public:
    typedef std::shared_ptr<HttpResponse> Pointer;
    enum Status {
        eERROR = -1,
        eUNKNOWN = 0,
        eBAD_REQUEST = 1,
        eNOT_FOUND,
        eOK,
        eSIZE
    };

    HttpResponse();
    static Pointer create() { return std::make_shared<HttpResponse>(); }
    void parse(const std::string &content);
    void set_status_line(const std::string& version, int status_code, const std::string& status);
    void set_version(const std::string& version) { version_ = version; }
    void set_status(const std::string& status) { status_ = status; }
    void set_status_code(int code) { status_code_ = code; }
    CookieBuilder set_cookie();
    std::string status_str() const;
    std::string version() const;
    int status_code() const;
    std::string to_string() const;

    std::string data() const { return data_; }
    void set_data(const std::string& data) { data_ = data;  }
    void append_data(const std::string& data) { data_.append(data); }
    bool empty() const { return status_code_ == 0; }
    //operator bool() const { return status_code_ != 0; }

private:
    std::vector<Cookie> cookies_;
    std::string version_;
    std::string status_;
    std::string data_;
    int status_code_;
};

class CookieBuilder {
public:
    CookieBuilder(HttpResponse& r) : key_(),value_(),path_(),domain_(), expires_(),response_(r) {}
    ~CookieBuilder() {
        if (!key_.empty() && !value_.empty()) {
            response_.cookies_.push_back(Cookie(key_, value_, path_, domain_, expires_));
        }
    }
    CookieBuilder& key(const std::string& key) {
        key_ = key;
        return *this;
    }
    CookieBuilder& value(const std::string& value) {
        value_ = value;
        return *this;
    }
    CookieBuilder& path(const std::string& path) {
        path_ = path;
        return *this;
    }
    CookieBuilder& domain(const std::string& domain) {
        domain_ = domain;
        return *this;
    }
    CookieBuilder& expires(const std::string& expires) {
        expires_ = expires;
        return *this;
    }
private:
    std::string key_;
    std::string value_;
    std::string path_;
    std::string domain_;
    std::string expires_;
    HttpResponse& response_;
};



HttpResponse default_response();

HttpResponse bad_request();

HttpResponse not_found();

HttpResponse ok_response();

std::string content_type(const std::string &path);

}//namespace
}//namespace

#endif // HTTPRESPONSE_H
