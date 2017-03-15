#include "http_server.h"
#include "../utils.h"
#include "http_request.h"
#include "http_response.h"
namespace net {
namespace http{

HttpServer::HttpServer(EventLoop *loop, std::size_t port) :
    loop_(loop),
    port_(port),
    listener_(loop_, port),
    buf_(1024),
    conns_(),
    handler_(HttpHandler::create())
{
    listener_.set_new_connection_callback([this] (Connection::Pointer conn) {
        conns_.insert(conn);
        loop_->start_coroutine(
                    std::bind(&HttpServer::handle_connection, this, std::placeholders::_1, conn));
    });
    listener_.listen();
}

void HttpServer::handle_connection(coroutine::Coroutine *co, Connection::Pointer conn) {
    std::string msg;
    while (conn->status() == Connection::eCONNECTING) {
        LOG_DEBUG<<"handle_connection";
        HttpRequest request = get_request_head(co, conn, msg);
        LOG_DEBUG<<request.to_string();
        update_request_raw_data(co, conn, request);
        assert(handler_);
        LOG_DEBUG<<"handle_request";
        handler_->handle_request(request, conn);
    } //while
}

HttpRequest HttpServer::get_request_head(coroutine::Coroutine *co,
                                         Connection::Pointer conn,
                                         std::string &msg)
{
    std::size_t n = 0;
    msg = "";
    do {
        n = utils::read_once(conn, Buffer(buf_), co);
        std::string recv(buf_.data(), n);
        //LOG_DEBUG<<recv;
        msg.append(recv);
        if (conn->status() != Connection::eCONNECTING) {
            //todo
            break;
        }
        if (msg.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    } while (1);

    HttpRequest request;
    request.parse(msg);
    return request;
}

void HttpServer::update_request_raw_data(coroutine::Coroutine *co,
                                      Connection::Pointer conn,
                                         HttpRequest &request) {
    std::size_t n = 0;
    int content_len = utils::to_int(request["Content-Length"]);
    int need_read_total = content_len - (int)request.raw_data().size();
    LOG_DEBUG<<"len:"<<content_len;
    LOG_DEBUG<<"need_read_total:"<<need_read_total;

    std::string data;
    while (1) {
        if (need_read_total <= 0) {
            //todo
            break;
        }
        if (conn->status() != Connection::eCONNECTING) {
            //todo
            break;
        }
        std::size_t need_read = std::min((std::size_t)need_read_total, buf_.size());
        n = utils::read_at_least(conn, Buffer(buf_), need_read, co);
        if (n != need_read) {
            //todo
            break;
        }
        std::string recv(buf_.data(), n);
        data += recv;
        need_read_total -= n;
    }
    request.append_raw_data(data);
}




}//namespace
}//namespace
