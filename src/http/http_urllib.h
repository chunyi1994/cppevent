#ifndef HTTPCONNECTION
#define HTTPCONNECTION
#include "http_request.h"
#include "http_response.h"
#include "../logging/logger.h"
#include "../utils.h"
#include "../socket.h"
#include "utility"
#include <netdb.h>
#include <arpa/inet.h>

namespace net {
namespace http{

static bool is_ipv4_addr(const std::string& name) {
    std::vector<std::string> result = utils::split(name, '.');
    if (result.size() != 4) {
        return false;
    }
    for (auto& str : result) {
        if (!utils::is_digit(str)) {
            return false;
        }
    }
    return true;
}

static int get_host_by_name(const std::string& name, std::string& ip) {
    if (is_ipv4_addr(name)) {
        ip = name;
        return 0;
    }
    struct hostent *hptr;
    char **pptr;
    char str[32];
    if ( (hptr = ::gethostbyname(name.c_str()) ) == NULL ) {
        LOG_DEBUG<<"err";
        return -1; /* 如果调用gethostbyname发生错误，返回1 */
    }
    pptr = hptr->h_addr_list;
    ::inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
    ip = str;
    return 0;
}

static int parse_url(std::string url, std::string& name, std::string& path, std::size_t& port) {
    if (utils::has_prefix(url, "http://")) {
        url = url.substr(7, url.length() - 7);
        port = 80;
    } else if (utils::has_prefix(url, "https://")) {
        url = url.substr(8, url.length() - 8);
        port = 443;
    }
    //www.cccc.com:9999/fdafafa  去掉/fdafafa部分
    std::size_t pos = url.find("/");
    if (pos != std::string::npos) {
        path = url.substr(pos, url.length() - pos);
        url = url.substr(0, pos);
    } else {
        path = "/";
    }
    pos = url.find(":");
    if (pos == std::string::npos) {
        name = url;
        return 0;
    }
    if (pos + 1 >= url.length()) {
        LOG_DEBUG<<"err";
        return -1;
    }
    std::string portstr = url.substr(pos + 1, url.length() - (pos + 1));
    port = (std::size_t)utils::to_int(portstr);
    if (port == 0) {
        LOG_DEBUG<<"err";
        return -1;
    }
    name = url.substr(0, pos);
    return 0;
}
enum HttpResultType {
    eOK,
    eNET_ERROR ,
    eURL_ERROR,
    eIP_ERROR ,
    eREAD_ERROR,
    eSEND_ERROR,
    eCONNECT_ERROR,
    eRESPONSE_ERROR,
    eTOO_MANY_REDIRECT
};

static HttpResultType request_server(
        const std::string& url,
        Socket& s,
        HttpRequest& request) {
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

    ErrorCode code = connect(s, ip, port);
    if (code.code() != ErrorType::eOK) {
        LOG_DEBUG<<"connect:"<<ip;
        return eCONNECT_ERROR;
    }
    //todo
    request.set_path(path);
    request["Host"] = name;


    request["Content-Length"] = utils::to_string(request.body().length());
    //发送request报文
    std::string str = request.to_string();
    LOG_DEBUG<<str;
    err = send(s, Buffer(str));
    if (err < 0) {
        return eSEND_ERROR;
    }


    if (request.method() == "POST" && !request.body().empty()) {
        str = request.body();
        err = send(s, Buffer(str));
        LOG_DEBUG<<str;
        if (err < 0) {
            return eSEND_ERROR;
        }
    }
    return eOK;
}

static bool is_redirect(int code) {
    return code == 301 || code == 302;
}

static HttpResultType handle_chunked(
        std::string content,
        Socket& s,
        HttpResponse::Pointer response ) {
    int err;
    int len;
    std::size_t pos;
    std::string need_read;
    while (len != 0) {
        pos = content.find("\r\n");
        if (pos == std::string::npos) {
            err = read_until(s, content, pos,"\r\n");
            if (err <= 0) {
                LOG_DEBUG<<"read error";
                return eREAD_ERROR;
            }
        }
        need_read = content.substr(0, pos);
        if (need_read.empty()) {
            LOG_DEBUG<<"empty";
            break;
        }
        if (!utils::is_digit_16(need_read)) {
            LOG_DEBUG<<"chunked error";
            return eRESPONSE_ERROR;
        }
        len = utils::hex2int(need_read);
        if (len == 0) {
            break;
        }
        int left_bytes = len - (content.length() - pos - 2);
        if (left_bytes < 0) {
            LOG_DEBUG<<"chunked error";
            return eRESPONSE_ERROR;
        }
        err = read_at_least(s, content, left_bytes);
        if (err < 0) {
            LOG_DEBUG<<"chunked error";
            return eRESPONSE_ERROR;
        }
        response->append_body(content.substr(pos + 2, len));
        content.erase(0, len + need_read.length() + 2 + 2);
    } // while()
    return eOK;
}

static HttpResultType handle_content_length(
        std::string content,
        Socket& s,
        HttpResponse::Pointer response) {
    int err;

    int response_all_len = utils::to_int(response->header("Content-Length"));
    int need_read_len = response_all_len - content.length();
    err = read_at_least(s, content, (std::size_t)need_read_len);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err";
        return eREAD_ERROR;
    }
    response->append_body(content);
    return eOK;
}

//todo 没有处理cookie
static std::pair<HttpResultType, HttpResponse::Pointer>
urlopen(std::string url,
        HttpRequest& request,
        int redirect_times = 5) {
    int err = 0;
    std::string response_all;
    HttpResponse::Pointer response = HttpResponse::create();
    Socket s;
    std::size_t pos;
    do {
        HttpResultType result = request_server(url, s, request);
        if (result != eOK) {
            return std::make_pair(result, nullptr);
        }
        response_all = "";
        //接收server发送来的消息，直到读到\r\n\r\n再返回,以区分头部和正文
        err = read_until(s, response_all, pos , "\r\n\r\n");
        if (err < 0) {
            //todo 服务器可能关闭了socket，或者网络问题
            LOG_DEBUG<<"err";
            return std::make_pair(eREAD_ERROR, nullptr);
        }
        response->parse(response_all.substr(0, pos));
        std::string content = response_all.substr(pos + 4, response_all.length() - (pos + 4));
        //服务器响应正文形式分两种，chunked和content-length
        if (response->header("Transfer-Encoding") == "chunked") {
            handle_chunked(content, s, response);
        } else{
            handle_content_length(content, s, response);
        }
        //处理重定向
        if (is_redirect(response->status_code())) {
            redirect_times --;
            url = response->header("Location");
            request.set_body("");
            request.set_method("GET");
        }
    } while (is_redirect(response->status_code()) && redirect_times > 0);
    if (redirect_times < 0) {
        //重定向次数超过限制，返回
        LOG_DEBUG<<"too many redirect";
        return std::make_pair(eTOO_MANY_REDIRECT, response);
    }
    LOG_DEBUG<<response->body();
    return std::make_pair(eOK, response);
}

//阻塞方式
//todo 不支持percent编码
static std::pair<HttpResultType, HttpResponse::Pointer>
get(const std::string& url, int redirect_times = 5) {
    HttpRequest request;
    default_request(request);
    request.set_method("GET");
    return urlopen(url, request, redirect_times);
}


//阻塞方式
//todo 不支持percent编码
static std::pair<HttpResultType, HttpResponse::Pointer>
post(const std::string& url,
     const std::map<std::string, std::string>& datas ,
     int redirect_times = 5) {
    HttpRequest request;
    default_request(request);
    std::string data = parse_data(datas);
    request.set_method("POST");
    request.set_body(data);
    if (!data.empty()) {
        request["Content-Type"] = "application/x-www-form-urlencoded";
    }
    return urlopen(url, request, redirect_times);
}

}//namespace
}//namespace
#endif // HTTPCONNECTION

