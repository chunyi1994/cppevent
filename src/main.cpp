#include <iostream>
#include "test/dotest.h"

#include "event_loop.h"
#include "linux_signal.h"
#include "logging/logger.h"


using namespace std;
using namespace net;
int main()
{

    test::do_test();
    net::EventLoop loop;
    std::size_t port = 23333;
    net::TcpServer server(&loop, port);
    //sigint
    Signal::signal(SIGINT, [&loop]{
        LOG_DEBUG<<"signal: sigint";
        loop.quit();

    });

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
        conn->send(recv);
    });

    LOG_DEBUG<<"server start!.";
    server.start();
    loop.loop();
    return 0;
}






