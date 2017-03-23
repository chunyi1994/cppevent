#include "response_writer.h"
#include "../utils.h"
#include "../logging/logger.h"
namespace net {
namespace http{


ResponseWriter::ResponseWriter(HttpConnection::Pointer conn, HttpResponse response) :
    conn_(conn),
    response_(std::move(response)),
    stream_()
{

}

ResponseWriter::~ResponseWriter() {
    std::string body = stream_.str();
    response_["Content-Length"] = utils::to_string(body.length());
    std::string header = response_.to_string();
    conn_->send(header);
    conn_->send(body);
}

void ResponseWriter::write(const std::string &msg) {
    stream_.write(msg.data(), msg.length());
}

void ResponseWriter::wirite(const char *msg, std::size_t n) {
    stream_.write(msg, n);
}

void ResponseWriter::set_header(const std::string &key, const std::string &value) {
    response_[key] = value;
}

CookieBuilder ResponseWriter::set_cookie() {
    return response_.set_cookie();
}

void ResponseWriter::redirect(const std::string &)
{

}

}//namespace
}//namespace

