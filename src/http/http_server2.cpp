#include "http_server2.h"
#include "event_loop.h"
#include "../utils.h"
#include "../linux_signal.h"
#include <fstream>
namespace net {
namespace http{

enum HttpErrType {
    eBAD_REQUEST,
    eOK
};

HttpServer2::HttpServer2(EventLoop* loop, std::size_t port):
    loop_(loop),
    port_(port),
    server_(loop, port),
    conns_(),
    buf_(1024 * 3),
    get_callbacks_(),
    post_callbacks_()
{
    Signal::signal(SIGPIPE, [] () {
        LOG_DEBUG<<"sigpipe";
    });

    server_.on_connection([this] (const Connection::Pointer& conn) {
        conns_[conn] = HttpConnection::create(conn);
    });

    server_.on_error([this] (const Connection::Pointer& conn, const net::ErrorCode& code) {
        LOG_DEBUG<<"on error:"<< code.msg();
        remove(conn);
    });

    server_.on_message([this] (const Connection::Pointer& c) {
        HttpConnection::Pointer conn = conns_[c];
        int state = conn->state;
        switch (state) {
        case eRECV_HEADER:
            on_recv_header(conn);
            break;
        case eRECV_BODY:
            on_recv_body(conn);
            break;
        default:
            break;
        }
    });
    server_.on_close([this] (const Connection::Pointer& conn) {
        remove(conn);
    });

}

void HttpServer2::start() {
    LOG_INFO<<"Http server start!";
    server_.start();
}

void HttpServer2::on_get(const std::string & path, const HttpServer2::HandleCallback &cb) {
    get_callbacks_[path] = cb;
}

void HttpServer2::on_post(const std::string &path, const HttpServer2::HandleCallback &cb) {
    post_callbacks_[path] = cb;
}

void HttpServer2::remove(const Connection::Pointer& conn) {
    auto iter = conns_.find(conn);
    if (iter == conns_.end()) {
        return;
    }
    remove(iter->second);
}

void HttpServer2::remove(HttpConnection::Pointer httpconn) {
    if (httpconn->state == HttpState::eDELETED) {
        return;
    }
    auto conn = httpconn->conn.lock();
    if (!conn) {
        return;
    }

    auto iter = conns_.find(conn);
    assert(iter != conns_.end());
    conns_.erase(iter);
    httpconn->state = HttpState::eDELETED;
}

void HttpServer2::on_recv_header(HttpConnection::Pointer httpconn) {
    auto& buf = httpconn->buf();
    std::size_t pos = buf.find("\r\n\r\n");
    if (pos == std::string::npos) {
        return;
    }
    std::string header = buf.read(pos + 4);
    //httpconn->buf.erase(0, pos + 4);
    httpconn->request.parse(header);
    httpconn->state = eRECV_BODY;
    on_recv_body(httpconn);
}

void HttpServer2::on_recv_body(HttpConnection::Pointer httpconn) {
    //todo 没考虑chunked
    int len ;
    if (httpconn->request["Transfer-Encoding"] == "chunked") {
        //todo
    } else {
        std::string content_len = httpconn->request["Content-Length"];
        if (content_len.empty()) {
            len = 0;
        } else if (utils::is_digit(content_len)) {
            len = utils::to_int(content_len);
        } else {
            handle_bad_request(httpconn);
            remove(httpconn);
            return;
        }
    }
    RingBuffer& buf = httpconn->buf();
    int left_bytes = len - static_cast<int>(buf.size());
    if (left_bytes > 0) {
        return;
    }
    std::string content = buf.read(len);
    // httpconn->buf.erase(0, content.length());
    httpconn->request.append_body(content);
    httpconn->state = eRECV_HEADER;
    handle_request(httpconn);
    if (!buf.empty()) {
        on_recv_header(httpconn);
    }
}

void HttpServer2::handle_request(HttpConnection::Pointer conn) {
    if (conn->err == eBAD_REQUEST) {
        handle_bad_request(conn);
        return;
    }
    HttpRequest& request = conn->request;
    std::string method = request.method();
    std::string path = request.path();
    std::size_t pos;

    if (method == "GET") {
        pos = path.find("?");
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }
        auto iter = get_callbacks_.find(path);
        if (iter != get_callbacks_.end()) {
            ResponseWriter response_writer(conn, ok_response());
            iter->second(conn->request, response_writer);
            return;
        }
        handle_static_file(conn);
    } else if (method == "POST") {
        auto iter = post_callbacks_.find(path);
        LOG_DEBUG<<"path:"<<path;
        if (iter != post_callbacks_.end()) {
            LOG_TRACE<<"1";
            ResponseWriter response_writer(conn, ok_response());
            iter->second(conn->request, response_writer);
            return;
        }
        LOG_TRACE<<"2";
        handle_404(conn);
    } else {
        handle_bad_request(conn);
        return;
    }
}

void HttpServer2::handle_404(HttpConnection::Pointer conn) {
    HttpResponse response;
    std::string ret = "not_found";
    response = not_found();
    response["Content-Length"] = utils::to_string(ret.size());
    response["Connection"] = "close";
    conn->send(response.to_string());
    conn->send(ret);
}

void HttpServer2::handle_bad_request(HttpConnection::Pointer conn) {
    HttpResponse response;
    std::string ret = "bad_request";
    response = bad_request();
    response["Content-Type"] = utils::to_string(ret.size());
    response["Connection"] = "close";
    conn->send(response.to_string());
    conn->send(ret);
}

const std::string static_folder_path("static/");
void HttpServer2::handle_static_file(HttpConnection::Pointer conn) {
    HttpRequest & request = conn->request;
    HttpResponse response;
    if (request.path().empty()) {
        handle_bad_request(conn);
        return ;
    }
    std::string path = request.path().erase(0, 1);
    path = static_folder_path + path;
    std::ifstream ifs(path,  std::ios::binary);
    if (!ifs) {
        handle_404(conn);
        return ;
    }
    response = ok_response();
    int filesize = utils::file_size(path.data());
    response["Content-Length"] = utils::to_string(filesize);
    response["Content-Type"] = content_type(path);
    conn->send(response.to_string());

    int nread = 0;
    while (!ifs.eof()) {
        if (nread == filesize) {
            break;
        }
        ifs.readsome(buf_.data(), buf_.size());
        nread +=  ifs.gcount();
        conn->send(buf_.data(), ifs.gcount());
    }
    return;
}

}//namespace
}//namespace
