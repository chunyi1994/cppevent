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
    eRECV_BODY,
    eDELETED,
};
class HttpServer2;
struct HttpConnection {
    typedef std::shared_ptr<HttpConnection> Pointer;

    static Pointer create(const Connection::Pointer& c) {
        return std::make_shared<HttpConnection>(c);
    }

    explicit HttpConnection(const Connection::Pointer& c) :
        conn(c), state(eRECV_HEADER), request() , response(), err(eOK) , url(){}

    //void shutdown() { conn->shutdown(); }

    void send(const std::string& msg) ;
    void send(const char* msg, std::size_t len);
    RingBuffer& buf() ;
    const RingBuffer& buf() const ;
public:
    Connection::WeakPointer conn;
    int state;
    HttpRequest request;
    HttpResponse response;
    int err;
    std::string url;
    Any context;
};

inline void HttpConnection::send(const std::string &msg) {
    Connection::Pointer connptr = conn.lock();
    assert(connptr);
    connptr->send(msg.data(), msg.length());
}

inline void HttpConnection::send(const char *msg, std::size_t len) {
    Connection::Pointer connptr = conn.lock();
    assert(connptr);
    connptr->send(msg, len);
}

inline RingBuffer &HttpConnection::buf() {
    Connection::Pointer connptr = conn.lock();
    assert(connptr);
    return connptr->buf();
}

inline const RingBuffer &HttpConnection::buf() const {
    Connection::Pointer connptr = conn.lock();
    assert(connptr);
    return connptr->buf();
}

}//naemspace
}//namespace
#endif // HTTP_CONNECTION_H

