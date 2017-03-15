#ifndef TEST_CONNECTOR
#define TEST_CONNECTOR
#include "test.h"
#include "../event_loop.h"
#include "../connector.h"
#include "../tcp_server.h"
#include "../utils.h"
#include "../tcp_client.h"
namespace test {

static void start_server(net::TcpServer& server) {

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

    server.set_message_callback([](const net::Connection::ConstPointer& conn, net::Buffer buffer, std::size_t bytes) {
        std::string recv(buffer.data(), bytes);
        DEBUG_INFO<<"server recv:"<<recv;
        conn->send("hello client!");
    });

    server.start();
}

static void test_tcpclient() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    start_server(server);
    net::TcpClient client(&loop);
    std::vector<char> buffer(1024);
    int err = client.connect("127.0.0.1", 23333, [&buffer](net::Connection::Pointer conn) {
            conn->send("hello server!");
            conn->read(net::Buffer(buffer), [](const net::Connection::Pointer &, net::Buffer buffer,std::size_t bytes) {
                std::string recv(buffer.data(), bytes);
                DEBUG_INFO<<"!!!!!connector conn recv:"<<recv;
                EXCEPT_EQ(recv, "hello client!");
            });
    });
    EXCEPT_TRUE(err >= 0);
    loop.loop();
}

static void test_connector() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    start_server(server);
    net::Connector connector(&loop);
    net::Connection::Pointer conn = connector.connect("127.0.0.1", 23333);
    utils::setnonblocking(conn->fd());
    EXCEPT_TRUE(conn);
    std::vector<char> buf(1024);
    conn->read(net::Buffer(buf), [&buf](const net::Connection::Pointer &, net::Buffer buffer, std::size_t bytes) {
        std::string recv(buffer.data(), bytes);
        DEBUG_INFO<<"connector conn recv:"<<recv;
        EXCEPT_EQ(recv, "hello client!");
    });

    conn->send("hello server!");
    loop.loop();
}
} //namespace
#endif // TEST_CONNECTOR

