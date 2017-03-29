#ifndef TEST_TCP_SERVER
#define TEST_TCP_SERVER
#include "test.h"
#include "../event_loop.h"
#include "../tcp_server.h"
// echo tcp server
namespace test {

static void test_tcp_server() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    server.on_connection([](const net::Connection::Pointer& conn) {
        EXCEPT_TRUE(conn->status() == net::Connection::eCONNECTING);
    });

    server.on_close([](const net::Connection::ConstPointer& conn) {
        EXCEPT_TRUE(conn->status() == net::Connection::eCLOSE);
    });

    server.on_error([](const net::Connection::Pointer&, const net::ErrorCode&) {
        DEBUG_INFO<<" error";
    });

    server.on_message([](const net::Connection::Pointer& conn) {
        std::string recv = conn->buf().read_all();
        conn->send(recv);
    });

    //如果在10秒内客户端不发送心跳，则关闭
    server.set_heartbeat_time(10);

    server.start();
    loop.loop();
}
} //namespace
#endif // TEST_TCP_SERVER

