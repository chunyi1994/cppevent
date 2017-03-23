#ifndef TEST_HTTP_SERVER
#define TEST_HTTP_SERVER
#include "test.h"
#include "../event_loop.h"
//#include "../http/http_server.h"
#include "../http/http_server2.h"

#include "../linux_signal.h"
namespace test {
//httpserver1
static void test_http_server() {
}

//httpserver2
static void test_http_server2() {
    net::EventLoop loop;
    net::Signal::signal(SIGINT, [&loop]{
        LOG_DEBUG<<"signal: sigint";
        loop.quit();
    });
    net::http::HttpServer2 server(&loop, 8000);
    server.on_get("/", [] (net::http::HttpRequest& request, net::http::ResponseWriter& writer) {
        request.parse_data();
        writer.write("n");
        writer.write(request.form("msg"));
        writer.write(request.form("msg2"));
        if (request.cookie("mycookie2") == "") {
            writer.set_cookie().key("mycookie2").value("mycookievalue");
        } else {
            DEBUG_INFO<<request.cookie("mycookie2");
        }
    });

    server.on_post("/", [] (net::http::HttpRequest& request, net::http::ResponseWriter& writer) {
        request.parse_data();
        writer.write("n");
        writer.write(request.form("msg"));
        writer.write(request.form("msg2"));
    });
    server.start();
    loop.loop();
}
} //namespace
#endif // TEST_HTTP_SERVER

