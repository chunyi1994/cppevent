#include <iostream>
#include "test/dotest.h"

#include "event_loop.h"
#include "linux_signal.h"
#include "logging/logger.h"


using namespace std;
using namespace net;

void example();
int main()
{

    test::do_test();
    //example();
    return 0;
}

void example() {
    {
       net::EventLoop loop;
       std::size_t port = 23333;
       net::TcpServer server(&loop, port);
       //sigint
       Signal::signal(SIGINT, [&loop]{
           LOG_DEBUG<<"signal: sigint";
           loop.quit();

       });
       server.on_connection([](const net::Connection::ConstPointer& conn) {
           if (conn->status() == net::Connection::eCLOSE) {
               DEBUG_INFO<<"close";
               // handle close
           } else if (conn->status() == net::Connection::eCONNECTING) {
               DEBUG_INFO<<" new connection";
           }
       });



       server.on_error([](const net::Connection::Pointer&, net::ErrorCode) {
           DEBUG_INFO<<" error";
       });

       server.on_message([](const net::Connection::Pointer& conn) {
           std::string recv = conn->buf().read_all();
           conn->send(recv);
       });

       LOG_DEBUG<<"server start!.";
       server.start();
       loop.loop();
   }

}




