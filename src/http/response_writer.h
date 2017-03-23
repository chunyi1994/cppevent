#ifndef RESPONSEWRITER_H
#define RESPONSEWRITER_H
#include <sstream>
#include "../listener.h"
#include "http_response.h"
#include "http_connection.h"
namespace net {
namespace http{

class ResponseWriter {
public:
    ResponseWriter(HttpConnection::Pointer conn, HttpResponse response);
    ~ResponseWriter();
    void write(const std::string& msg);
    void wirite(const char* msg, std::size_t n);
    void set_header(const std::string& key, const std::string& value);
    CookieBuilder set_cookie();
    void redirect(const std::string& location);
private:
    HttpConnection::Pointer conn_;
    HttpResponse response_;
    std::stringstream stream_;
};

}//namespace
}//namespace
#endif // RESPONSEWRITER_H
