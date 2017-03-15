#ifndef TEST_HTTP_SERVER
#define TEST_HTTP_SERVER
#include "test.h"
#include "../event_loop.h"
#include "../http/http_server.h"
namespace test {
    static void test_http_server() {
        net::EventLoop loop;
        net::http::HttpServer server(&loop, 8000);
        loop.loop();
    }
} //namespace
#endif // TEST_HTTP_SERVER

