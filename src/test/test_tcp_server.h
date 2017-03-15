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
    server.set_connection_callback([](const net::Connection::ConstPointer& conn) {
        if (conn->status() == net::Connection::eCLOSE) {
            DEBUG_INFO<<"close";
            // handle close
        } else if (conn->status() == net::Connection::eCONNECTING) {
            DEBUG_INFO<<" new connection";
        }
    });

    server.set_error_callback([](const net::Connection::ConstPointer&) {
        DEBUG_INFO<<" error";
    });

    server.set_message_callback([](const net::Connection::ConstPointer& conn,
                                net::Buffer buffer,
                                std::size_t bytes) {
        std::string recv(buffer.data(), bytes);
        conn->send(recv);
    });

    server.start();
    loop.loop();
}
} //namespace
#endif // TEST_TCP_SERVER

