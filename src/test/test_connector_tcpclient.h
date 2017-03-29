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

    server.on_connection([](const net::Connection::ConstPointer& conn) {
        if (conn->status() == net::Connection::eCLOSE) {
            DEBUG_INFO<<"close";
            // handle close
        } else if (conn->status() == net::Connection::eCONNECTING) {
            DEBUG_INFO<<" new connection";
        }
    });

    server.on_error([](const net::Connection::ConstPointer&, net::ErrorCode) {
        DEBUG_INFO<<" error";
    });

    server.on_message([](const net::Connection::Pointer& conn) {
        std::string recv = conn->buf().read_all();
        DEBUG_INFO<<"server recv:"<<recv;
        conn->send("hello client!");
    });

    server.start();
}

//static void test_tcpclient() {
//    net::EventLoop loop;
//    net::TcpServer server(&loop, 23333);
//    start_server(server);
//    net::TcpClient client(&loop);
// //   std::vector<char> buffer(1024);
////    int err = client.connect("127.0.0.1", 23333, [&buffer](net::Connection::Pointer conn) {
////            conn->send("hello server!");
////            conn->on_read(net::Buffer(buffer), [](const net::Connection::Pointer &, net::Buffer buffer,std::size_t bytes) {
////                std::string recv(buffer.data(), bytes);
////                DEBUG_INFO<<"!!!!!connector conn recv:"<<recv;
////                EXCEPT_EQ(recv, "hello client!");
////            });
////    });
//    //EXCEPT_TRUE(err >= 0);
//    loop.loop();
//}

//阻塞式
static void test_connector() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    start_server(server);
    net::Connector connector(&loop);
    net::Connection::Pointer conn = connector.connect("127.0.0.1", 23333);
    net::setnonblocking(conn->fd());
    EXCEPT_TRUE(conn);
    conn->on_read([](const net::Connection::Pointer &c) {
        std::string recv =  c->buf().read_all();
        DEBUG_INFO<<"connector conn recv:"<<recv;
        EXCEPT_EQ(recv, "hello client!");
        DEBUG_INFO<<"from server:" <<recv;
    });

    conn->send("hello server!");
    loop.loop();
}

//非阻塞式
static void test_connector2() {
    net::EventLoop loop;
    net::TcpServer server(&loop, 23333);
    start_server(server);
    net::Connector connector(&loop);
    net::Connection::Pointer conn_ptr;
    connector.connect("127.0.0.11", 2333, [&conn_ptr] (const net::Connection::Pointer & conn, net::ErrorCode error_code) {
        conn_ptr = conn;
        EXCEPT_TRUE(conn);

        DEBUG_INFO<<"err!!!"<<error_code.msg();
        conn->on_read( [] (const net::Connection::Pointer &c) {
            std::string recv = c->buf().read_all();
            DEBUG_INFO<<"connector conn recv:"<<recv;
            EXCEPT_EQ(recv, "hello client!");
        });
        conn->send("hello server!");
    });

    loop.loop();
}

} //namespace
#endif // TEST_CONNECTOR

