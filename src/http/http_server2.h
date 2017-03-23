#ifndef HTTPSERVER2_H
#define HTTPSERVER2_H
#include <map>
#include <sstream>
#include "../tcp_server.h"
#include "http_request.h"
#include "http_response.h"
#include "http_connection.h"
#include "response_writer.h"
namespace net {
namespace http{

//纯异步实现的http server

class HttpServer2
{
public:
    typedef std::function<void(HttpRequest&, ResponseWriter&)> HandleCallback;
public:
    HttpServer2(EventLoop* loop, std::size_t port);
    void start();
    void on_get(const std::string& path, const HandleCallback& cb) ;
    void on_post(const std::string& path, const HandleCallback& cb) ;
    std::size_t size() const { return conns_.size(); }
    std::size_t port() const { return port_; }
    const TcpServer* server() const { return &server_; }
private:
    void on_recv_header(HttpConnection::Pointer httpconn);
    void on_recv_data(HttpConnection::Pointer httpconn);

    void handle_request(HttpConnection::Pointer httpconn);
    void handle_404(HttpConnection::Pointer httpconn);
    void handle_bad_request(HttpConnection::Pointer httpconn);
    void handle_static_file(HttpConnection::Pointer);

    void remove(Connection::ConstPointer conn);
    void remove(HttpConnection::Pointer httpconn);
private:
    EventLoop* loop_;
    std::size_t port_;
    TcpServer server_;
    std::map<Connection::ConstPointer, HttpConnection::Pointer> conns_;
    std::vector<char> buf_;
    std::map<std::string, HandleCallback> get_callbacks_;
    std::map<std::string, HandleCallback> post_callbacks_;
};
}//namespace
}//namespace
#endif // HTTPSERVER2_H
