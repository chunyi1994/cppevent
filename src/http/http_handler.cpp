#include "http_handler.h"
#include <fstream>
#include <sys/stat.h>
#include "../utils.h"
#include "../logging/logger.h"
namespace net {
namespace http {



HttpHandler::HttpHandler() :
    static_folder_path_("./")
{
}

void HttpHandler::handle_request(HttpRequest & request, Connection::Pointer conn) {
    if (request.method() == "GET") {
        handle_static_file(request, conn);
        return;
    }
    LOG_TRACE<<"2";
    std::string ret = "bad_request";
    HttpResponse response = bad_request();
    response["Content-Type"] = utils::to_string(ret.size());
    conn->send(response.to_string());
    conn->send(ret);
}

HttpResponse HttpHandler::default_response() {
    HttpResponse response;
    response.set_status_line("HTTP/1.1", 200, "OK");
    response["Server"] = "Cppevent Server 1.0";
    response["Content-Type"] = "text/html";
    return response;
}

HttpResponse HttpHandler::bad_request() {
    HttpResponse response = default_response();
    response.set_status_line("HTTP/1.1", 400, "Bad Request");
    return response;
}

HttpResponse HttpHandler::not_found() {
    HttpResponse response = default_response();
    response.set_status_line("HTTP/1.1", 404, "Not found");
    return response;
}

int HttpHandler::handle_static_file(HttpRequest & request, Connection::Pointer conn) {
    LOG_TRACE<<"1";
    HttpResponse response;

    if (request.path().empty()) {
        LOG_TRACE<<"3";
        std::string ret = "bad_request";
        response = bad_request();
        response["Content-Type"] = utils::to_string(ret.size());
        conn->send(response.to_string());
        conn->send(ret);
        return 0;
    }
    std::string path = request.path().erase(0, 1);
    path = static_folder_path_ + path;
    std::fstream ifs(path);
    if (!ifs) {
        LOG_TRACE<<"4";
        std::string ret = "not_found";
        response = not_found();
        response["Content-Type"] = utils::to_string(ret.size());
        conn->send(response.to_string());
        conn->send(ret);
        return 0;
    }
    LOG_TRACE<<"5";
    response = default_response();
    int filesize = file_size(path.data());
    LOG_DEBUG<<"filesize："<<filesize;
    response["Content-Length"] = utils::to_string(filesize);
    response["Content-Type"] = content_type(path);
    LOG_DEBUG<<response.to_string();
    conn->send(response.to_string());
    std::string line;
    while (std::getline(ifs, line)) {
        LOG_DEBUG<<line;
        conn->send(line);
    }
    LOG_TRACE<<"6";
    return 0;
}



ssize_t HttpHandler::file_size(const char *path) {
    ssize_t fileSize = -1;
    struct stat statBuff;
    if(stat(path, &statBuff) >= 0) {
        fileSize = statBuff.st_size;
    }
    return fileSize;
}

std::string HttpHandler::content_type(const std::string &path) {
    size_t dotPos = path.find(".");
    if(dotPos == std::string::npos){
        return "text/html";
    }
    std::string ext = path.substr(dotPos, 4);
    if (ext == ".png") {
        return "image/png";
    } else if (ext == ".css") {
        return "text/css";
    } else if (ext == ".jpg") {
        return "image/jpeg";
    } else if (ext == ".gif") {
        return "image/gif";
    }  else if (ext == ".htm") {
        return "text/html";
    }  else {
        return "text/html";
    }
}

} //namespace
} //namespace
