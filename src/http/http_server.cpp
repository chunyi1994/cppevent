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
        conn->set_error_callback([this] (Connection::Pointer conn) {
            LOG_DEBUG<<"handle error";
            remove(conn);
        });

        conn->set_close_callback([this](Connection::Pointer conn) {
            LOG_DEBUG<<"handle close";
            remove(conn);
        });

        loop_->start_coroutine(
                    std::bind(&HttpServer::handle_connection, this, std::placeholders::_1, conn));
    });
    listener_.listen();
}

void HttpServer::remove(Connection::Pointer conn) {
    auto iter = conns_.find(conn);
    assert(iter != conns_.end());
    conns_.erase(iter);
}

void HttpServer::handle_connection(coroutine::Coroutine *co, Connection::Pointer conn) {
    std::string msg;
    while (conn->status() == Connection::eCONNECTING) {
        HttpRequest request;
        int err = update_request_head(co, conn, msg, request);
        if (err < 0) {
            LOG_DEBUG<<"err<0";
            break;
        }
        err = update_request_raw_data(co, conn, request);
        assert(handler_);
        handler_->handle_request((HttpResponse::Status)err, request, conn);
    } //while
}

int HttpServer::update_request_head(coroutine::Coroutine *co,
                                    Connection::Pointer conn,
                                    std::string &msg,
                                    HttpRequest& request) {
    std::size_t n = 0;
    msg = "";
    do {
        n = utils::read_once(conn, Buffer(buf_), co);
        std::string recv(buf_.data(), n);
        //LOG_DEBUG<<recv;
        msg.append(recv);
        if (conn->status() != Connection::eCONNECTING) {
            return HttpResponse::eERROR;
        }
        if (msg.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    } while (1);

    request.parse(msg);
    return HttpResponse::eUNKNOWN;
}

int HttpServer::update_request_raw_data(coroutine::Coroutine *co,
                                         Connection::Pointer conn,
                                         HttpRequest &request) {
    std::size_t n = 0;
    int content_len = utils::to_int(request["Content-Length"]);
    int need_read_total = content_len - (int)request.raw_data().size();
    std::string data;
    while (1) {
        if (need_read_total < 0) {
            return HttpResponse::eBAD_REQUEST;
        }
        if (need_read_total == 0) {
            return HttpResponse::eUNKNOWN;
        }
        if (conn->status() != Connection::eCONNECTING) {
            return HttpResponse::eERROR;
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
    return HttpResponse::eUNKNOWN;
}




}//namespace
}//namespace
