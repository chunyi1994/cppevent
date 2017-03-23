#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include <memory>
#include "../connection.h"
#include "http_request.h"
#include "http_response.h"
namespace net {
namespace http {
enum HttpState{
    eRECV_HEADER,
    eRECV_DATA,
    eDELETED,
};
class HttpServer2;
struct HttpConnection {
    typedef std::shared_ptr<HttpConnection> Pointer;

    static Pointer create(const Connection::ConstPointer& c) {
        return std::make_shared<HttpConnection>(c);
    }

    HttpConnection(const Connection::ConstPointer& c) :
        conn(c) , buf(), state(eRECV_HEADER), request() , err(eOK){}

    //void shutdown() { conn->shutdown(); }

    void send(const std::string& msg) { conn->send(msg.data(), msg.length()); }
    void send(const char* msg, std::size_t len) { conn->send(msg, len); }

public:
    Connection::ConstPointer conn;
    std::string buf;
    int state;
    HttpRequest request;
    int err;
};

}//naemspace
}//namespace
#endif // HTTP_CONNECTION_H

