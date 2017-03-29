#ifndef TEST_TCPCLIENT
#define TEST_TCPCLIENT
#include "test.h"
#include "../event_loop.h"
#include "../tcp_client.h"
#include "../tcp_server.h"
namespace test {

static void start_tcp_server22(net::TcpServer& server) {
    server.on_connection([](const net::Connection::ConstPointer& conn) {
        EXCEPT_TRUE(conn->status() == net::Connection::eCONNECTING);
    });

    server.on_close([](const net::Connection::Pointer& conn) {
        EXCEPT_TRUE(conn->status() == net::Connection::eCLOSE);
    });

    server.on_error([](const net::Connection::Pointer&, const net::ErrorCode&) {
        DEBUG_INFO<<" error";
    });

    server.on_message([](const net::Connection::Pointer& conn) {
        std::string recv = conn->buf().read_all();
        DEBUG_INFO<<"recv:"<<recv;
        EXCEPT_EQ("hello", recv);
        conn->send(recv);
    });

    //如果在10秒内客户端不发送心跳，则关闭
   // server.set_heartbeat_time(10);

    server.start();
}
static void test_tcpclient() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    start_tcp_server22(server);
    net::TcpClient client(&loop);
    client.on_connect("127.0.0.1", 23333, [] (
                      const net::Connection::Pointer& conn, net::ErrorCode ) {
        DEBUG_INFO<<"on_connect";
        DEBUG_INFO<<conn->address().ip<<":"<<conn->address().port;
        conn->send("hello");
    });

    client.on_connect("127.0.0.1", 24444, [] (
                      const net::Connection::Pointer& conn, net::ErrorCode err) {
        DEBUG_INFO<<"on_connect";
        DEBUG_INFO<<err.msg();
        EXCEPT_FALSE(conn);
      //  DEBUG_INFO<<conn->address().ip<<":"<<conn->address().port;
        // conn->send("hello");
    });
    loop.loop();
}
} //namespace
#endif // TEST_TCPCLIENT

