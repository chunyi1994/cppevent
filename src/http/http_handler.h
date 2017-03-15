#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H
#include "http_request.h"
#include "http_response.h"
#include "../connection.h"
#include <memory>
namespace net {
namespace http {

class HttpHandler {
public:
    typedef std::shared_ptr<HttpHandler> Pointer;

public:
    HttpHandler();
    void set_path(const std::string& path) { static_folder_path_ = path; }
    static std::string content_type(const std::string &path);
    static ssize_t file_size(const char*);
    static Pointer create() { return std::make_shared<HttpHandler>(); }
    virtual void handle_request(HttpRequest&, Connection::Pointer conn);
protected:
    HttpResponse default_response();
    HttpResponse bad_request();
    HttpResponse not_found();
    int handle_static_file(HttpRequest & request, Connection::Pointer conn);
private:
    std::string static_folder_path_;
};

} //namespace
} //namespace
#endif // HTTP_HANDLER_H

