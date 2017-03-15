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

void HttpHandler::handle_request(HttpResponse::Status status,
                                 HttpRequest & request,
                                 Connection::Pointer conn) {
    if (status == HttpResponse::eBAD_REQUEST) {
        std::string ret = "bad_request";
        HttpResponse response = bad_request();
        response["Content-Length"] = utils::to_string(ret.size());
        conn->send(response.to_string());
        conn->send(ret);
        return;
    }
    if (request.method() == "GET") {
        handle_static_file(request, conn);
        return;
    }
    std::string ret = "bad_request";
    HttpResponse response = bad_request();
    response["Content-Length"] = utils::to_string(ret.size());

    conn->send(response.to_string());
    conn->send(ret);
}



int HttpHandler::handle_static_file(HttpRequest & request, Connection::Pointer conn) {
    HttpResponse response;

    if (request.path().empty()) {
        std::string ret = "bad_request";
        response = bad_request();
        response["Content-Type"] = utils::to_string(ret.size());
        conn->send(response.to_string());
        conn->send(ret);
        return 0;
    }
    std::string path = request.path().erase(0, 1);
    path = static_folder_path_ + path;
    std::ifstream ifs(path,  std::ios::binary);
    if (!ifs) {
        std::string ret = "not_found";
        response = not_found();
        response["Content-Length"] = utils::to_string(ret.size());
        conn->send(response.to_string());
        conn->send(ret);
        return 0;
    }
    response = default_response();
    int filesize = file_size(path.data());
    response["Content-Length"] = utils::to_string(filesize);
    response["Content-Type"] = content_type(path);
    conn->send(response.to_string());
    std::vector<char> buf(1024);
    int nread = 0;
    while (!ifs.eof()) {
        if (nread == filesize) {
            break;
        }
        ifs.readsome(buf.data(), 1024);
        nread +=  ifs.gcount();
        conn->send(buf.data(), ifs.gcount());
    }
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
