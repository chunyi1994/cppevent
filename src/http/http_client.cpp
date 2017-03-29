#include "http_client.h"
namespace net {
namespace http{
HttpClient::HttpClient(EventLoop *loop) :
    loop_(loop),
    conns_(),
    client_(TcpClient::create(loop_))
{

}

int init_request(const std::string& url, HttpRequest& request) {
    std::string name;
    std::string ip;
    std::string path;
    std::size_t port;
    int err = parse_url(url, name, path, port);
    if (err < 0) {
        LOG_DEBUG<<"parse_url err";
        return eURL_ERROR;
    }
    err = get_host_by_name(name, ip);
    if (err < 0) {
        LOG_DEBUG<<"get_host_by_name err";
        return eIP_ERROR;
    }
    LOG_DEBUG<<"get_host_by_name:"<<ip;
    //todo
    request.set_path(path);
    request.set_version("HTTP/1.1");
    request["Host"] = name;
    request["Content-Length"] = utils::to_string(request.body().length());
    return eOK;
}

void HttpClient::on_get(const std::string &url, ConnectionCallback cb) {
    std::string name;
    std::string ip;
    std::string path;
    std::size_t port;
    int err = parse_url(url, name, path, port);
    if (err < 0) {
        LOG_DEBUG<<"parse_url err";
        //return eURL_ERROR;
    }
    err = get_host_by_name(name, ip);
    if (err < 0) {
        LOG_DEBUG<<"get_host_by_name err";
        //return eIP_ERROR;
    }
    LOG_DEBUG<<"get_host_by_name:"<<ip;

    HttpRequest request;
    request.set_method("GET");
    init_request(url, request);
    client_->on_read([this] (const Connection::Pointer & conn) {
        auto iter = conns_.find(conn);
        assert(iter != conns_.end());
        HttpConnection::Pointer httpconn =  iter->second;
        int status = httpconn->state;
        switch (status) {
        case eRECV_HEADER:
            handle_recv_header(httpconn);
            break;
        case eRECV_BODY:
            handle_recv_body(httpconn);
            break;
        }
    });

    client_->on_connect(ip, port, [this, cb, request] (Connection::Pointer conn,  ErrorCode err) {
        if (err.code() != eOK) {
            if (cb) {
                cb(nullptr, err);
            }
            return;
        }
        HttpConnection::Pointer httpconn = HttpConnection::create(conn);
        httpconn->context = 0;
        conns_[conn] = httpconn;
        httpconn->send(request.to_string());
        httpconn->request = request;
    });

    client_->on_close([this](Connection::Pointer conn) {
        auto iter = conns_.find(conn);
        if (iter != conns_.end()) {
            conns_.erase(iter);
        }
    });
}

void HttpClient::handle_recv_header(HttpConnection::Pointer httpconn) {
    auto& buf = httpconn->buf();
    std::size_t pos = buf.find("\r\n\r\n");
    if (pos == std::string::npos) {
        return;
    }
    std::string header = buf.read(pos + 4);
    httpconn->response.parse(header);
    httpconn->state = eRECV_BODY;
    handle_recv_body(httpconn);
}

const int kUNFINISHED = 0;
const int kFINISHED = 1;
const int kRESPONSE_ERROR = -1;

void HttpClient::handle_recv_body(HttpConnection::Pointer httpconn) {
    int result;
    if (httpconn->request["Transfer-Encoding"] == "chunked") {
        result = chunked_helper(httpconn);
    } else {
        result = content_length_helper(httpconn);
    }
    //todo
    if (result == kFINISHED) {

    } else if(result == kRESPONSE_ERROR) {

    }
    //handle_request(httpconn);
    if (!httpconn->buf().empty()) {
        handle_recv_header(httpconn);
    }
}

int HttpClient::content_length_helper(HttpConnection::Pointer httpconn) {
    int len ;
    std::string content_len = httpconn->request["Content-Length"];
    if (content_len.empty()) {
        len = 0;
    } else if (utils::is_digit(content_len)) {
        len = utils::to_int(content_len);
    } else {
        //todo
        return kRESPONSE_ERROR;
    }
    RingBuffer& buf = httpconn->buf();
    int left_bytes = len - static_cast<int>(buf.size());
    if (left_bytes > 0) {
        return kUNFINISHED;
    }
    std::string content = buf.read(len);
    httpconn->response.append_body(content);
    httpconn->state = eRECV_HEADER;
    return kFINISHED;
}

int HttpClient::chunked_helper(HttpConnection::Pointer httpconn) {
    RingBuffer& buf = httpconn->buf();
    int len = any_cast<int>(httpconn->context);
    if (len > 0) {
        if (static_cast<int>(httpconn->buf().size()) < len) {
            return kUNFINISHED;
        }
        httpconn->response.append_body(httpconn->buf().read(len));
    } else {
        std::size_t pos = buf.find("\r\n");
        if (pos == std::string::npos) {
            return kUNFINISHED;
        }
        std::string need_read = buf.read(pos);
        buf.erase(2);  //\r\n
        if (need_read.empty()) {
            LOG_DEBUG<<"empty";
            return kFINISHED;
        }
        if (!utils::is_digit_16(need_read)) {
            LOG_DEBUG<<"chunked error";
            return kRESPONSE_ERROR;
        }
        int len = utils::hex2int(need_read);
        if (len == 0) {
            //todo
            return kFINISHED;
        }
        httpconn->context = len;
    }
    return kUNFINISHED;
}

}//namespaxe
}//namespace
