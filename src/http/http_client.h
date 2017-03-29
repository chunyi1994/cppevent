#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include "../tcp_client.h"
#include "http_urllib.h"
#include "http_connection.h"
#include <map>
#include <functional>
namespace net {

namespace http{

class HttpClient
{
public:
    typedef std::function<void(HttpConnection::Pointer,
                               ErrorCode)> ConnectionCallback;
public:
    explicit HttpClient(EventLoop* loop);
    void on_get(const std::string& url, ConnectionCallback cb);

private:
    void handle_recv_header(HttpConnection::Pointer conn);
    void handle_recv_body(HttpConnection::Pointer conn);
    int content_length_helper(HttpConnection::Pointer conn);
    int chunked_helper(HttpConnection::Pointer conn);

private:
    EventLoop* loop_;

    std::map
    <Connection::WeakPointer,
        HttpConnection::Pointer,
        std::owner_less<Connection::WeakPointer>
    > conns_;

    TcpClient::Pointer client_;
};



}//namespaxe
}//namespace

#endif // HTTPCLIENT_H
