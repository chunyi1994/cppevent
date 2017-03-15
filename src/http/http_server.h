#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <vector>
#include "../event_loop.h"
#include "../listener.h"
#include "http_request.h"
#include "http_response.h"
#include "http_handler.h"
#include <set>
namespace net {
namespace http{

class HttpServer
{
public:
    HttpServer(EventLoop* loop, std::size_t port);
    void set_handler(HttpHandler::Pointer handler);
    HttpHandler::Pointer handler() { return handler_; }
    std::size_t size() const { return conns_.size(); }
private:
    void remove(Connection::Pointer conn);
    void handle_connection(coroutine::Coroutine* co, Connection::Pointer conn);
    int update_request_head(coroutine::Coroutine* co,
                                 Connection::Pointer conn,
                                 std::string&msg,
                                HttpRequest& request);

    int update_request_raw_data(coroutine::Coroutine* co,
                                 Connection::Pointer conn,
                                 HttpRequest& request);

private:
    EventLoop* loop_;
    std::size_t port_;
    Listener listener_;
    std::vector<char> buf_;
    std::set<Connection::Pointer> conns_;
    HttpHandler::Pointer handler_;
};

}//namespace
}//namespace


#endif // HTTPSERVER_H
