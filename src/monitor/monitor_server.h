#ifndef MONITORSERVER_H
#define MONITORSERVER_H
#include "../http/http_server2.h"
#include "../utils.h"
#include "../logging/logger.h"
#include "cpuinfo.inc"
#include "status.inc"
#include <fstream>
#include <functional>
namespace net{
namespace monitor{
// http://127.0.0.1: <port>/status      查看进程状态
// http://127.0.0.1: <port>/cpuinfo   查看cpu状态
// http://127.0.0.1: <port>/                查看主页
//<port> 端口号，视情况而定。

template<class T>
class MonitorServer {
    typedef T ServerType;
public:
    MonitorServer(EventLoop* loop, std::size_t port, ServerType* server);
    void start();
    void handle_index(http::HttpRequest&, http::ResponseWriter& writer);
private:
    http::HttpServer2 http_server_;
    const ServerType* server_;
};

typedef MonitorServer<const TcpServer> TcpMonitor;

template<class T>
inline MonitorServer<T>::MonitorServer(
        EventLoop* loop,
        std::size_t port,
        ServerType* server) :
    http_server_(loop, port),
    server_(server)
{
    http_server_.on_get("/", std::bind(&MonitorServer<T>::handle_index, this,
                                       std::placeholders::_1, std::placeholders::_2));

    http_server_.on_get("/cpuinfo", handle_cpuinfo);
    http_server_.on_get("/status", handle_status);
}

template<class T>
inline void MonitorServer<T>:: start() {
    LOG_INFO<<"Monitor server start!";
    http_server_.start();
}

template <class T>
inline void MonitorServer<T>::handle_index(
        http::HttpRequest &, http::ResponseWriter &writer) {
    writer.write("<!DOCTYPE html><html lang=\"zh-cn\"><head>");
    writer.write("<meta charset=\"utf-8\"><title>infomation</title></head><body>");
    writer.write("<p>");
    writer.write("Server port:" + utils::to_string(server_->port()));
    writer.write("</p>");
    writer.write("<p>");
    writer.write("size:" + utils::to_string(server_->size()));
    writer.write("</p>");
    writer.write("<p>");
    writer.write("worked time:" + utils::to_string(server_->worked_time()));
    writer.write("</p>");
    writer.write("</body></html>");
}

}//namespae
}//namespace
#endif // MONITORSERVER_H
