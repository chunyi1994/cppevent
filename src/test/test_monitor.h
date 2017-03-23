#ifndef TEST_MONITOR_H
#define TEST_MONITOR_H
#include "test.h"
#include "../monitor/monitor_server.h"
#include "../http/http_server2.h"
#include "../linux_signal.h"
namespace test {
static void test_monitor() {
    net::EventLoop loop;
    net::Signal::signal(SIGINT, [&loop]{
        LOG_DEBUG<<"signal: sigint";
        loop.quit();
    });

    //启动一个server
    net::http::HttpServer2 server(&loop, 8000);
    server.on_get("/", [] (net::http::HttpRequest&, net::http::ResponseWriter& writer) {
        writer.write("nihao!");
    });


    //启动monitor
    net::monitor::TcpMonitor monitor(&loop, 9000, server.server());
    monitor.start();
    server.start();
    loop.loop();
}
} //namespace
#endif // TEST_MONITOR_H

