#ifndef MONITORSERVER_H
#define MONITORSERVER_H
#include "../http/http_server2.h"
#include "../utils.h"
#include "../logging/logger.h"
namespace net{
namespace monitor{

template<class T>
class MonitorServer {
    typedef T ServerType;
public:
    MonitorServer(EventLoop* loop, std::size_t port, ServerType* server);
    void start();
private:
    http::HttpServer2 http_server_;
    const ServerType* server_;
};

typedef MonitorServer<const TcpServer> TcpMonitor;

template<class T>
MonitorServer<T>::MonitorServer(EventLoop* loop, std::size_t port, ServerType* server) :
    http_server_(loop, port),
    server_(server)
{
    http_server_.on_get("/", [this](http::HttpRequest&, http::ResponseWriter& writer) {
        writer.write("Server port:" + utils::to_string(server_->port()));
         writer.write("\r\n");
         writer.write("size:" + utils::to_string(server_->size()));
         writer.write("\r\n");
         writer.write("worked time:" + utils::to_string(server_->worked_time()));
    });
}

template<class T>
void MonitorServer<T>:: start() {
    LOG_INFO<<"Monitor server start!";
    http_server_.start();
}

//template<class T>
//static MonitorServer<T> make_monitor(EventLoop* loop, std::size_t port, T* server) {
//    return MonitorServer<T>(loop, port, server);
//}

}//namespae
}//namespace
#endif // MONITORSERVER_H
